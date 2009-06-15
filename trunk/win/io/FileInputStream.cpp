// FileInputStream.cpp

#include "FileInputStream.h"
#include <exception/IOException.h>
#include <util/Locker.h>
#include <util/WinUtil.h>


FileInputStream::FileInputStream()
{
	m_handle = INVALID_HANDLE_VALUE;
}

FileInputStream::FileInputStream(HANDLE handle)
{
	m_handle = handle;
}

FileInputStream::~FileInputStream()
{
	close();
}

void FileInputStream::open(const String fileName)
{
	Locker locker(m_mutex);

	if (m_handle != INVALID_HANDLE_VALUE)
	{
		close();
	}

	// Pass null pointer to get buffer size
	uint32 pathLen = GetFullPathName(fileName.c_str(), 0, NULL, NULL);

	if (pathLen == 0)
	{
		throw IOException(String("Failed to open \"") + fileName +
			"\", couldn't get path : " + WinUtil::getLastErrorMessage());
	}

	// Allocate buffer on the stack
	uint32 fullPathLen = pathLen + fileName.length() + 1;

	char* fullPath = (char*)_alloca(fullPathLen);

	// Fill with the real path
	pathLen = GetFullPathName(fileName.c_str(), fullPathLen, fullPath, NULL);

	if (pathLen == 0)
	{
		throw IOException(String("Failed to open \"") + fileName +
			"\", couldn't get path : " + WinUtil::getLastErrorMessage());
	}

	m_handle = CreateFile(fullPath, // File name
						  GENERIC_READ, // We only need to read
						  FILE_SHARE_READ | FILE_SHARE_WRITE, // Only block deletion
						  NULL, // Default security attributes
						  OPEN_EXISTING, // Only open if it exists (don't create)
						  FILE_FLAG_SEQUENTIAL_SCAN, // Flag that we will scan sequentially
						  NULL); // No template file handle

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		throw IOException(String("Failed to open \"") + fileName +
			"\" : " + WinUtil::getLastErrorMessage());
	}
}

int32 FileInputStream::read()
{
	char tempBuffer[1];
	int64 ret = internalRead(tempBuffer, 1);

	if (ret == -1)
		return -1;
	return tempBuffer[0];
}

void FileInputStream::close()
{
	Locker locker(m_mutex);

	if (m_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_handle);
		m_handle = INVALID_HANDLE_VALUE;
	}
}

int64 FileInputStream::read(void *buffer, uint32 len)
{
	return internalRead((int8*)buffer, len);
}

int64 FileInputStream::internalRead(int8* buffer, uint32 len)
{
	Locker locker(m_mutex);

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		throw IOException("Cannot read from closed stream");
	}

	uint32 bytesRead = 1;
	uint32 readSuccess = true;

	SetLastError(ERROR_SUCCESS);

	readSuccess = ReadFile(m_handle, // The handle to read from
						   buffer, // Pointer to our buffer
						   len, // Bytes to try to read
						   &bytesRead, // ReadFile will set the number of bytes read
						   NULL); // Not using asynchronous I/O

	// If we failed to read anything, check for closed pipe
	if (!readSuccess || bytesRead == 0)
	{
		uint32 lastError = GetLastError();
		if (lastError == ERROR_SUCCESS ||
			lastError == ERROR_BROKEN_PIPE ||
			lastError == ERROR_HANDLE_EOF)
		{
			return -1; // pipe done - normal exit path.
		}
		else
		{
			throw IOException(String("Failed to read stream: ") +
				WinUtil::getLastErrorMessage());
		}
	}

	return bytesRead;
}
