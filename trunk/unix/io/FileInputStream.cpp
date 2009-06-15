// FileInputStream.cpp

#include "FileInputStream.h"
#include <exception/IOException.h>
#include <util/Locker.h>
#include <util/UnixUtil.h>

#include <errno.h> // For errno defines
#include <fcntl.h> // For create flags
#include <sys/ioctl.h> // For ioctl()

// Where FIONREAD is defined varries
#ifdef __CYGWIN__
#	include <sys/socket.h>
#else
#	include <sys/filio.h>
#endif


FileInputStream::FileInputStream()
{
	m_fileDescriptor = -1;
}

FileInputStream::FileInputStream(int32 fileDescriptor)
{
	m_fileDescriptor = fileDescriptor;
}

FileInputStream::~FileInputStream()
{
	close();
}

void FileInputStream::open(const String fileName)
{
	Locker locker(m_mutex);

	if (m_fileDescriptor != -1)
	{
		close();
	}

	// Read access, create if needed
	int flags = O_RDONLY | O_CREAT;

	// Support large files if the OS supports it
#ifdef O_LARGEFILE
	flags |= O_LARGEFILE;
#endif

	m_fileDescriptor = UnixUtil::sys_open(fileName.c_str(), // Name of file
										  flags, // Creation flags, see above
										  0666); // Permissions mask if created

	if (m_fileDescriptor == -1)
	{
		throw IOException(String("Failed to open file: ") +
			UnixUtil::getLastErrorMessage());
	}
}

void FileInputStream::close()
{
	Locker locker(m_mutex);

	if (m_fileDescriptor != -1)
	{
		UnixUtil::sys_close(m_fileDescriptor);
		m_fileDescriptor = -1;
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

int64 FileInputStream::read(void* buffer, uint32 len)
{
	return internalRead(buffer, len);
}

int64 FileInputStream::internalRead(void* buffer, uint32 len)
{
	Locker locker(m_mutex);

	if (m_fileDescriptor == -1)
	{
		throw IOException("Failed to read from stream: Stream is closed");
	}

	size_t bytesAvail = 0;

	// If you read from a redirected std handle of a child process read will
	// block forever if it is called after the input is complete. However, ioctl
	// correctly returns EPIPE, so we call it to check for validity.
	//
	// An alternative to this method is to use non-blocking reads, polling until
	// data is returned or waitpid indicates the process is dead.

	// FIONREAD asks how many bytes are available. We ignore the result.
	if (ioctl(m_fileDescriptor, FIONREAD, &bytesAvail) == -1)
	{
		if (errno == EPIPE)
			return -1;
	}

	errno = 0;
	ssize_t bytesRead = UnixUtil::sys_read(m_fileDescriptor, buffer, (size_t)len);

	// If we failed to read anything, check for closed pipe
	if (bytesRead == -1)
	{
		uint32 lastError = errno;
		if (lastError == 0 ||
			lastError == EPIPE)
		{
			return -1;
		}
		else
		{
			throw IOException(String("Failed to read from stream: ") +
				UnixUtil::getLastErrorMessage());
		}
	}

	// Some unix varients just keep returning 0 at end of pipe
	if (bytesRead == 0)
		return -1;

	return bytesRead;
}
