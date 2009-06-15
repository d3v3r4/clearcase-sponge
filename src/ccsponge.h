// base.h

#ifndef BASE_H
#define BASE_H

// Comment out this line and no logging code will compile
#define ENABLE_LOGGING

// Logging with a level below this will be ignored (1 to 3)
#define LOG_LEVEL 0

// Ensure WINDOWS is defined if on Windows
#if !defined(WINDOWS)
#	if defined(WIN32) || defined(WIN64)
#		define WINDOWS
#	endif
#endif

// Otherwise we assume we are some Unix
#if !defined(WINDOWS)
#	define UNIX
#endif

// Standard Windows imports and defines
// Note that we do not want to import windows.h more than once
#if defined(WINDOWS)
#	define _CRT_SECURE_NO_WARNINGS // Skip the "secure" CRT warnings
#	define NO_VTABLE __declspec(novtable) // Don't add vtable instructions for the class
#	include <windows.h>
#else
#	define NO_VTABLE // Do nothing under GCC
#endif

// windows.h defines some of the types below on Windows, so we just pick up
// the versions from the Microsoft header.

// Standard signed types
#if !defined(int8)
#	define int8 char
#endif
#if !defined(int16)
#	define int16 short
#endif
#if !defined(int32)
#	if defined(UNIX)
#		define int32 int
#	else
#		define int32 long
#	endif
#endif
#if !defined(int64)
#	if defined(UNIX)
#		define int64 long long
#	else
#		define int64 __int64
#	endif
#endif

// Standard unsigned types
#if !defined(uint8)
#	define uint8 unsigned char
#endif
#if !defined(uint16)
#	define uint16 unsigned short
#endif
#if !defined(uint32)
#	if defined(UNIX)
#		define uint32 unsigned int
#	else
#		define uint32 unsigned long
#	endif
#endif
#if !defined(uint64)
#	if defined(UNIX)
#		define uint64 unsigned long long
#	else
#		define uint64 unsigned __int64
#	endif
#endif

#endif // BASE_H
