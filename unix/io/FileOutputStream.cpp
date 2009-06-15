// FileOutputStream.cpp

#include "FileOutputStream.h"
#include <exception/IOException.h>
#include <util/Locker.h>
#include <util/UnixUtil.h>

#include <errno.h> // For error defines
#include <fcntl.h> // For create flags


FileOutputStream::FileOutputStream()
{
	m_fileDescriptor = -1;
}

FileOutputStream::FileOutputStream(int32 fileDescriptor)
{
	m_fileDescriptor = fileDescriptor;
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

	if (m_fileDescriptor != -1)
	{
		close();
	}

	// Write access, create if needed
	int flags = O_WRONLY | O_CREAT;

	// Support large files if the OS supports it
#ifdef O_LARGEFILE
	flags |= O_LARGEFILE;
#endif

	// If passed in append, mask on the append option,
	// otherwise truncate the file
	if (append)
	{
		flags |= O_APPEND;
	}
	else
	{
		flags |= O_TRUNC;
	}

	m_fileDescriptor = UnixUtil::sys_open(fileName.c_str(), // File name
										  flags, // Creation flags, see above
										  0666); // File mask if created

	if (m_fileDescriptor == -1)
	{
		throw IOException(String("Failed to open file: ") +
			UnixUtil::getLastErrorMessage());
	}
}

void FileOutputStream::close()
{
	Locker locker(m_mutex);

	if (m_fileDescriptor != -1)
	{
		UnixUtil::sys_close(m_fileDescriptor);
		m_fileDescriptor = -1;
	}
}

int32 FileOutputStream::write(int32 byte)
{
	char tempBuffer[1];
	tempBuffer[0] = (char)(byte & 0x000000ff);

	return (int32)internalWrite(tempBuffer, 1);
}

int64 FileOutputStream::write(const void* buffer, uint32 maxlen)
{
	return internalWrite(buffer, maxlen);
}

int64 FileOutputStream::internalWrite(const void* buffer, uint32 maxlen)
{
	Locker locker(m_mutex);

	if (m_fileDescriptor == -1)
	{
		throw IOException("Failed to write to stream: Stream is closed");
	}

	errno = 0;
	ssize_t bytesWritten = UnixUtil::sys_write(m_fileDescriptor, buffer, maxlen);

	// If we failed to write anything, check for closed pipe
	if (bytesWritten == -1)
	{
		uint32 lastError = errno;
		if (lastError == 0 ||
			lastError == EPIPE)
		{
			return -1;
		}
		else
		{
			throw IOException(String("Failed to write to stream: ") +
				UnixUtil::getLastErrorMessage());
		}
	}
	return bytesWritten;
}
