// SystemException.h

#ifndef SYSTEM_EXCEPTION_H
#define SYSTEM_EXCEPTION_H

#include <text/String.h>
class String;

#include <stdexcept>
using namespace std;
 
class SystemException : public runtime_error
{
public:
	SystemException(const char* msg) : runtime_error(msg) { }
	SystemException(const String msg) : runtime_error(msg.c_str()) { }
};

#endif // SYSTEM_EXCEPTION_H
