// ParsingException.h

#ifndef PARSING_EXCEPTION_H
#define PARSING_EXCEPTION_H

#include <text/String.h>
class String;

#include <stdexcept>
using namespace std;
 
class ParsingException : public runtime_error
{
public:
	ParsingException(const char* msg) : runtime_error(msg) { }
	ParsingException(const String& msg) : runtime_error(msg.c_str()) { }
};

#endif // PARSING_EXCEPTION_H
