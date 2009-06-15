// Help.cpp

#include "Help.h"

const char* HELP_TEXT =
"Extracts lines of code changed from ClearCase vob:\n\n"
"ccsponge pname ... "
"[-nomain] "
"[-nomerge] "
"[-after DATE] "
"[-before DATE] "
"[-period PERIOD] "
"[-users USER-LIST] "
"[-brtypes BRTYPE-LIST] "
"[-exts EXTENSION-LIST]"
"\n\nEnter -help [OPTION] for help on a specific option\n";

const char* EXTRA_PARAM_TEXT =
"You can only pass a single value into -help";

const char* BAD_HELP_TEXT =
"parameter to -help not recognized";

const char* HELP_HELP_TEXT =
"Try asking for help just one time";

const char* OUT_HELP_TEXT =
"-o -out\nSpecifies an output file name. If -o is not passed, a file "
"named \"sponge.out\" is created.";

const char* NOMAIN_HELP_TEXT =
"-nomain\nIgnores versions on the \"main\" branch. Useful if all "
"development is done off of \"main\" and you want to ignore drops. "
"Ignored if the -brtypes parameter is also passed.";

const char* NOMERGE_HELP_TEXT =
"-nomerge\nIgnores versions with a merge arrow to them. Useful for "
"ignoring rebase versions. Be cautious with this option as it may "
"ignore legitamate changes.";

const char* AFTER_HELP_TEXT =
"-after DATE\nIgnores versions before the specified DATE. The passed "
"date must be a date in a format clearcase recognizes. If -after is not "
"passed, no minimum date restriction is applied.";

const char* BEFORE_HELP_TEXT =
"-before DATE\nIgnores versions after the specified DATE. The passed "
"date must be a date in a format clearcase recognizes. If -before is not "
"passed, no maximum date restriction is applied.";

const char* PERIOD_HELP_TEXT =
"-period PERIOD\nSpecifies a period of time to write output data in. "
"Supported PERIOD values are: \"daily\", \"weekly\", \"monthly\". If "
"-period is not passed, defaults to \"weekly\"";

const char* USERS_HELP_TEXT =
"-users USER-LIST\nSpecifies a list of users to look for when examining "
"versions. Versions created by users that are not in the passed list will "
"be ignored. The list of users should be comma delimited: \"jsmith, "
"alincoln, bcosby\". If -users is not passed, any creator is accepted.";

const char* BRTYPES_HELP_TEXT =
"-brtypes BRTYPE-LIST\nSpecifies a list of clearcase brtypes to look for "
"when examining versions. Versions that are not on any of the passed "
"brtypes will be ignored. The BRTYPE-LIST should be comma delimited: "
"\"fixes1, fixes2, new_dev\". If -brtypes is not passed, all brtypes are "
"considered. Passing -brtypes causes the -nomain option to be ignored.";

const char* EXTS_HELP_TEXT =
"-exts EXTENSION-LIST\nSpecifies a list of file extensions when examining "
"versions. Versions that do not have any of the passed extensions will be "
"ignored. The EXTENSION-LIST should be comma delimited: \".h, .cpp,.hpp\". "
"If -exts is not passed, all extensions are allowed.";

bool Help::isHelpParam(String param)
{
	return (param.equalsIgnoringCase("h") ||
			param.equalsIgnoringCase("help") ||
			param.equalsIgnoringCase("-h") ||
			param.equalsIgnoringCase("-help") ||
			param.equalsIgnoringCase("--h") ||
			param.equalsIgnoringCase("--help"));
}

bool Help::isHelpRequest(Array<String>& parameters)
{
	// Return true if there are no arguments, need to tell user to ask for help
	if (parameters.size() == 1)
	{
		return true;
	}
	else if (parameters.size() >= 2)
	{
		String firstParam = parameters.get(1);

		// Tolerate all the usual pleas for mercy
		return isHelpParam(firstParam);
	}

	return false;
}

const char* Help::getHelpMessage(Array<String>& parameters)
{
	if (parameters.size() == 1)
	{
		return HELP_TEXT;
	}
	else if (parameters.size() > 3)
	{
		return EXTRA_PARAM_TEXT;
	}

	String param = parameters.get(1);

	if (isHelpParam(param) && parameters.size() == 2)
	{
		return HELP_TEXT;
	}

	param = parameters.get(2);

	if (param.startsWith('-'))
		param = param.subString(1);

	if (param.equals("help"))
	{
		return HELP_HELP_TEXT;
	}
	else if (param.equals("o") ||
			 param.equals("out"))
	{
		return OUT_HELP_TEXT;
	}
	else if (param.equals("nomain"))
	{
		return NOMAIN_HELP_TEXT;
	}
	else if (param.equals("nomerge"))
	{
		return NOMERGE_HELP_TEXT;
	}
	else if (param.equals("after"))
	{
		return AFTER_HELP_TEXT;
	}
	else if (param.equals("before"))
	{
		return BEFORE_HELP_TEXT;
	}
	else if (param.equals("period"))
	{
		return PERIOD_HELP_TEXT;
	}
	else if (param.equals("users"))
	{
		return USERS_HELP_TEXT;
	}
	else if (param.equals("brtypes"))
	{
		return BRTYPES_HELP_TEXT;
	}
	else if (param.equals("exts"))
	{
		return EXTS_HELP_TEXT;
	}
	else
	{
		return BAD_HELP_TEXT;
	}
}
