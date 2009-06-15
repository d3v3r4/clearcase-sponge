// IOException.h

#ifndef IO_EXCEPTION_H
#define IO_EXCEPTION_H

#include <text/String.h>
class String;

#include <stdexcept>
using namespace std;
 
class IOException : public runtime_error
{
public:
	IOException(const char* msg) : runtime_error(msg) { }
	IOException(const String msg) : runtime_error(msg.c_str()) { }
};

#endif // IO_EXCEPTION_H
