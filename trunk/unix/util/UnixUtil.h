// UnixUtil.h

#ifndef UNIX_UTIL_H
#define UNIX_UTIL_H

#include <ccsponge.h>
#include <text/String.h>

/*
 * Namespace for unix utility functions. Should only be used by unix only
 * files.
 *
 * Functions with names preceded by "sys_" are wrappers around system
 * calls to add retry mechanisms where required. Most unix OS variants allow
 * some system calls to be interupted or return when partly completed. User
 * code is required to handle the retry.
 */
namespace UnixUtil
{
	/*
	 * This will return a string that contains the system error message
	 * for the current last error code.
	 */
	String getLastErrorMessage();

	/*
	 * This will return an error message string for the passed error
	 * number.
	 */
	String getErrorMessage(uint32 errorNumber);

	// System call wrappers -------------------------------------------------

	int sys_open(const char* pathname, int flags, mode_t mode);

	int sys_close(int fd);

	ssize_t sys_read(int fd, void* buf, size_t nbyte);

	ssize_t sys_write(int fd, const void* buf, size_t nbyte);

	int sys_dup2(int oldfd, int newfd);
}

#endif // UNIX_UTIL_H
