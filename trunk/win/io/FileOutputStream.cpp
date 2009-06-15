// FileOutputStream.cpp

#include "FileOutputStream.h"
#include <exception/IOException.h>
#include <util/Locker.h>
#include <util/WinUtil.h>


FileOutputStream::FileOutputStream()
{
	m_handle = INVALID_HANDLE_VALUE;
	m_append = false;
}

FileOutputStream::FileOutputStream(HANDLE handle)
{
	m_handle = handle;
	m_append = false; // Cannot inherit the append mode
}

FileOutputStream::~FileOutputStream()
{
	close();
}

void FileOutputStream::open(const String fileName)
{
	open(fileName, false);
}

void FileOutputStream::open(const String fileName, bool append)
{
	Locker locker(m_mutex);

	if (m_handle != INVALID_HANDLE_VALUE)
	{
		close();
	}

	m_append = append;

	// Pass null pointer to get buffer size
	uint32 pathLen = GetFullPathName(fileName.c_str(), 0, NULL, NULL);

	if (pathLen == 0)
	{
		throw IOException(String("Failed to open file: ") + WinUtil::getLastErrorMessage());
	}

	// Allocate buffer on the stack
	uint32 fullPathLen = pathLen + fileName.length() + 1;

	char* fullPath = (char*)_alloca(fullPathLen);

	// Fill with the real path
	pathLen = GetFullPathName(fileName.c_str(), fullPathLen, fullPath, NULL);

	if (pathLen == 0)
	{
		throw IOException(String("Failed to open file: ") + WinUtil::getLastErrorMessage());
	}

	uint32 openMode;
	uint32 attrsAndFlags;

	// Change how we open the file depending on append flag
	if (append)
	{
		openMode = OPEN_ALWAYS; // Open file if it exists, create if not
		attrsAndFlags = FILE_FLAG_OVERLAPPED; // Doing asynchronous writes
	}
	else
	{
		openMode = CREATE_ALWAYS; // Creates or erases the existing file
		attrsAndFlags = FILE_ATTRIBUTE_NORMAL; // Normal synchronous writes
	}

	m_handle = CreateFile(fullPath, // File name
						  GENERIC_WRITE, // We only need to write
						  FILE_SHARE_READ | FILE_SHARE_WRITE, // Only block deletion
						  NULL, // Default security attributes
						  openMode, // See above
						  attrsAndFlags, // See above
						  NULL); // No template file handle

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		throw IOException(String("Failed to open file: ") + WinUtil::getLastErrorMessage());
	}
}

void FileOutputStream::close()
{
	if (m_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}

int32 FileOutputStream::write(int32 byte)
{
	char tempBuffer[1];
	tempBuffer[0] = (char)(byte & 0x000000ff);

	if (m_append)
		return (int32)writeAppend(tempBuffer, 1);
	return (int32)writeNormal(tempBuffer, 1);
}

int64 FileOutputStream::write(const void* buffer, uint32 maxlen)
{
	if (m_append)
		return writeAppend(buffer, maxlen);
	return writeNormal(buffer, maxlen);
}

int64 FileOutputStream::writeNormal(const void* buffer, uint32 maxlen)
{
	Locker locker(m_mutex);

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		throw IOException("Cannot write to closed stream");
	}

	uint32 bytesWritten = 0;
	uint32 writeSuccess;

	SetLastError(ERROR_SUCCESS);

	writeSuccess = WriteFile(m_handle, // The handle to write to
							 buffer, // The buffer of bytes to write
							 maxlen, // The number of bytes in the buffer
							 &bytesWritten, // WriteFile will set the number of bytes written
							 NULL); // Not using asynchronous I/O

	// If we failed to write anything, check for closed pipe
	if (!writeSuccess)
	{
		uint32 lastError = GetLastError();
		if (lastError == ERROR_SUCCESS ||
			lastError == ERROR_BROKEN_PIPE)
		{
			return -1;
		}
		else
		{
			throw IOException(String("Failed to write to stream: ") +
				WinUtil::getLastErrorMessage());
		}
	}
	return bytesWritten;
}

/*
 * Doing synchronous "atomic" appends to a file is a bit of a pain in
 * Windows. A bit of research showed that Java uses an undocumented method
 * of specifying FILE_APPEND_DATA access without FILE_WRITE_DATA
 * (see io_util_md.c) but this created issues because the removal of
 * FILE_WRITE_DATA permissions caused file locking and file truncation to
 * fail.
 *
 * Instead of trying some funky undocumented method of appending
 * synchronously, we use a documented method with asynchronous IO where we
 * ask it to append to the file by specifying an offset of 0xFFFFFFFFFFFFFFFF.
 * We then call GetOverlappedResult with a blocking flag so the function will
 * not return until the IO is complete.
 */
int64 FileOutputStream::writeAppend(const void* buffer, uint32 maxlen)
{
	Locker locker(m_mutex);

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		throw IOException("Cannot write to closed stream");
	}

	// To prevent collisions we need to duplicate the file handle.
	// Otherwise each thread needs a seperate FileOutputStream,
	// which is just as costly to copy.
	HANDLE dupHandle;
	BOOL dupSuccess = DuplicateHandle(GetCurrentProcess(), // Source process
									  m_handle, // Source handle
									  GetCurrentProcess(), // Destination process
									  &dupHandle, // Destination handle
									  0, // Desired access (ignored due to DUPLICATE_SAME_ACCESS)
									  FALSE, // Do not allow child process to inherit
									  DUPLICATE_SAME_ACCESS); // Use same file access rights

	if (!dupSuccess)
	{
		throw IOException(WinUtil::getLastErrorMessage());
	}

	// Create an OVERLAPPED struct
	OVERLAPPED overlapped;
	memset(&overlapped, 0, sizeof(OVERLAPPED));

	// Setting both Offset and OffsetHigh to 0xFFFFFFFF means "append"
	overlapped.Offset = 0xFFFFFFFF;
	overlapped.OffsetHigh = 0xFFFFFFFF;

	// Create an Event for the OVERLAPPED object to signal completion
	overlapped.hEvent = CreateEvent(NULL, // Default security
									TRUE, // Manual reset event (required for WriteFile)
									FALSE, // Start unsignaled (required for WriteFile)
									NULL); // Unnamed event

	if (overlapped.hEvent == NULL)
	{
		uint32 lastError = GetLastError();
		CloseHandle(dupHandle);
		throw IOException(WinUtil::getErrorMessage(lastError));
	}

	SetLastError(ERROR_SUCCESS);

	// Start the asynchronous write to the file
	uint32 writeSuccess;

	writeSuccess = WriteFile(dupHandle, // The handle to write to
							 buffer, // The buffer of bytes to write
							 maxlen, // The number of bytes in the buffer
							 NULL, // Can't get bytes written with asynchronous IO, see below
							 &overlapped); // Reference to OVERLAPPED struct

	if (!writeSuccess)
	{
		uint32 lastError = GetLastError();

		// WriteFile sometimes completes before returning and sometimes returns 0 and
		// sets the last error of the calling thread. ERROR_IO_PENDING indicates the
		// write is happily proceeding asynchronously.

		if (lastError != ERROR_IO_PENDING)
		{
			CloseHandle(dupHandle);
			CloseHandle(overlapped.hEvent);

			// ERROR_HANDLE_EOF just indicates we are at end of file.
			if (lastError == ERROR_HANDLE_EOF)
			{
				return -1;
			}
			else
			{
				throw IOException(WinUtil::getErrorMessage(lastError));
			}
		}
	}

	// Since we are using asynchronous IO we need to wait for the operation to complete.
	uint32 bytesWritten = 0;

	writeSuccess = GetOverlappedResult(m_handle, // Handle to wait on
									   &overlapped, // Reference to OVERLAPPED struct
									   &bytesWritten, // Pointer to int put bytes written into
									   TRUE); // Block until done writing

	if (!writeSuccess)
	{
		uint32 lastError = GetLastError();
		CloseHandle(dupHandle);
		CloseHandle(overlapped.hEvent);

		// ERROR_HANDLE_EOF just indicates we are at end of file.
		if (lastError == ERROR_HANDLE_EOF)
		{
			return -1;
		}
		else
		{
			throw IOException(WinUtil::getErrorMessage(lastError));
		}
	}

	CloseHandle(dupHandle);
	CloseHandle(overlapped.hEvent);

	return bytesWritten;
}
