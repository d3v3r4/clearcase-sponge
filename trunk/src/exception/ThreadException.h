// ThreadException.h

#ifndef THREAD_EXCEPTION_H
#define THREAD_EXCEPTION_H

#include <text/String.h>
class String;

#include <stdexcept>
using namespace std;
 
class ThreadException : public runtime_error
{
public:
	ThreadException(const char* msg) : runtime_error(msg) { }
	ThreadException(const String msg) : runtime_error(msg.c_str()) { }
};

#endif // THREAD_EXCEPTION_H
