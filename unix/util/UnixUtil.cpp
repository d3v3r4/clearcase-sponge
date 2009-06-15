// UnixUtil.cpp

#include "UnixUtil.h"

#include <errno.h> // For errno and error defines
#include <fcntl.h> // For open()
#include <string.h> // For strerror_r()
#include <unistd.h> // For dup2(), close(), read(), write()


// Size of buffer for holding error messages
#define ERROR_BUFFER_SIZE 512


String UnixUtil::getLastErrorMessage()
{
	return getErrorMessage(errno);
}

String UnixUtil::getErrorMessage(uint32 errorNumber)
{
	char msgBuffer[ERROR_BUFFER_SIZE];

	// If the last error is set to 0 (no error) return an empty string
	if (errorNumber == 0)
		return String();

	// TODO: may need to flip to strerror for Solaris

	// Thread safe retrieval of the error message string
	strerror_r(errorNumber, msgBuffer, ERROR_BUFFER_SIZE-1);

	return String(msgBuffer);
}

int UnixUtil::sys_open(const char* pathname, int flags, mode_t mode)
{
	int ret;
	do
	{
		ret = open(pathname, flags, mode);
	}
	while (ret == -1 && errno == EINTR);

	return ret;
}

int UnixUtil::sys_close(int fd)
{
	int ret;
	do
	{
		ret = close(fd);
	}
	while (ret == -1 && errno == EINTR);

	return ret;
}

ssize_t UnixUtil::sys_read(int fd, void* buf, size_t nbyte)
{
	ssize_t ret;

	do
	{
		ret = read(fd, buf, nbyte);
	}
	while (ret == -1 && errno == EINTR);

	return ret;
}

ssize_t UnixUtil::sys_write(int fd, const void* buf, size_t nbyte)
{
	ssize_t ret;

	do
	{
		ret = write(fd, buf, nbyte);
	}
	while (ret == -1 && errno == EINTR);

	return ret;
}

int UnixUtil::sys_dup2(int oldfd, int newfd)
{
	int ret;

	do
	{
		ret = dup2(oldfd, newfd);
	}
	while (ret == -1 && errno == EINTR);

	return ret;
}
