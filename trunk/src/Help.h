// Help.h

#ifndef HELP_H
#define HELP_H

#include <text/String.h>
#include <util/Array.h>

namespace Help
{
	bool isHelpParam(String param);
	bool isHelpRequest(Array<String>& parameters);
	const char* getHelpMessage(Array<String>& parameters);
}

#endif // HELP_H
