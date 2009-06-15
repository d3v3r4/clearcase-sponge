// WinUtil.cpp

#include "WinUtil.h"

String WinUtil::getLastErrorMessage()
{
	return getErrorMessage(GetLastError());
}

String WinUtil::getErrorMessage(uint32 errorNumber)
{
	char* msgBuffer;

    FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |	// Allocate a buffer for the string
		FORMAT_MESSAGE_FROM_SYSTEM |		// This is a system error message
        FORMAT_MESSAGE_IGNORE_INSERTS,		// Don't add insertion strings
        NULL,								// No source
        errorNumber,						// The message id (error number)
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // The language id (none essentially)
        (LPTSTR)&msgBuffer,					// The location to copy a pointer to the buffer to
        0,									// Buffer size (ignored)
		NULL);								// Argument list (ignored)

	// Make a string object
	String ret = String(msgBuffer);

	// Free the memory Windows allocated
	// LocalFree is nessesary due to the allocation method
	LocalFree(msgBuffer);

	return ret;
}
