// WinUtil.h

#ifndef WIN_UTIL_H
#define WIN_UTIL_H

#include <ccsponge.h>
#include <text/String.h>

/*
 * This namespace has a set of static methods for accessing and printing
 * error information.
 */
namespace WinUtil
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
};

#endif // WIN_UTIL_H