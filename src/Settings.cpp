// Settings.cpp

#include "Settings.h"

Settings::Settings()
{
	// Defaults
	m_excludeMerges = false;
	m_excludeMain = false;
	m_period = WEEKLY;
	m_outputFile = String("sponge.out");
}

Settings::Settings(const Settings& other)
{
	m_excludeMerges = other.m_excludeMerges;
	m_excludeMain = other.m_excludeMain;
	m_period = other.m_period;
	m_outputFile = other.m_outputFile;
	m_afterDate = other.m_afterDate;
	m_beforeDate = other.m_beforeDate;
	m_users = other.m_users;
	m_brtypes = other.m_brtypes;
	m_extensions = other.m_extensions;
	m_paths = other.m_paths;
}

Settings::~Settings()
{

}

bool Settings::populate(Array<String>& parameters, String& error)
{
	if (parameters.size() < 2)
	{
		error = "Missing required path parameter";
		return false;
	}

	uint32 index = 1; // Skip program name

	// First, add every parameter until one that starts with '-' to m_paths
	while (index < parameters.size())
	{
		String param = parameters.get(index);

		if (param.startsWith('-'))
			break;

		m_paths.push_back(param);
		index++;
	}

	// Now work our way through the rest of the options in a good old
	// parsing loop...

	while (index < parameters.size())
	{
		String param = parameters.get(index);

		if (param.equals("-nomain"))
		{
			m_excludeMain = true;
		}
		else if (param.equals("-nomerge"))
		{
			m_excludeMerges = true;
		}
		else if (param.equals("-o"))
		{
			if (index == parameters.size() - 1)
			{
				error = "Missing output filename after -o";
				return false;
			}

			index++;
			m_outputFile = parameters.get(index);
		}
		else if (param.equals("-after"))
		{
			if (index == parameters.size() - 1)
			{
				error = "Missing date after option -after";
				return false;
			}

			index++;
			m_afterDate = parameters.get(index);
		}
		else if (param.equals("-before"))
		{
			if (index == parameters.size() - 1)
			{
				error = "Missing date after option -before";
				return false;
			}

			index++;
			m_beforeDate = parameters.get(index);
		}
		else if (param.equals("-period"))
		{
			if (index == parameters.size() - 1)
			{
				error = "Missing date after option -period";
				return false;
			}

			index++;
			m_period = parsePeriod(parameters.get(index), error);

			if (error.length() > 0)
			{
				return false;
			}
		}
		else if (param.equals("-users"))
		{
			if (index == parameters.size() - 1)
			{
				error = "Missing user list after option -users";
				return false;
			}

			index++;
			parseList(parameters.get(index), m_users);
		}
		else if (param.equals("-brtypes"))
		{
			if (index == parameters.size() - 1)
			{
				error = "Missing brtype list after option -brtypes";
				return false;
			}

			index++;
			parseList(parameters.get(index), m_brtypes);
		}
		else if (param.equals("-exts"))
		{
			if (index == parameters.size() - 1)
			{
				error = "Missing extension list after option -ext";
				return false;
			}

			index++;
			parseExtensionList(parameters.get(index), m_extensions, error);

			if (error.length() > 0)
			{
				return false;
			}
		}
		else
		{
			error = String("Unknown parameter: ") + param + "\nUse -help for help";
			return false;
		}

		index++;
	}

	return true;
}

bool Settings::getMergesExcluded()
{
	return m_excludeMerges;
}

bool Settings::getMainExcluded()
{
	return m_excludeMain;
}

Settings::timePeriod Settings::getPeriod()
{
	return m_period;
}

String Settings::getOutputFile()
{
	return m_outputFile;
}

String Settings::getAfterDate()
{
	return m_afterDate;
}

String Settings::getBeforeDate()
{
	return m_beforeDate;
}

vector<String> Settings::getUsers()
{
	return m_users;
}

vector<String> Settings::getBrtypes()
{
	return m_brtypes;
}

vector<String> Settings::getExtensions()
{
	return m_extensions;
}

vector<String> Settings::getPaths()
{
	return m_paths;
}

Settings& Settings::operator=(const Settings& other)
{
	if (this == &other)
		return *this;

	m_excludeMerges = other.m_excludeMerges;
	m_excludeMain = other.m_excludeMain;
	m_period = other.m_period;
	m_afterDate = other.m_afterDate;
	m_beforeDate = other.m_beforeDate;
	m_users = other.m_users;
	m_brtypes = other.m_brtypes;
	m_extensions = other.m_extensions;
	m_paths = other.m_paths;
	return *this;
}

Settings::timePeriod Settings::parsePeriod(String value, String& error)
{
	if (value.equalsIgnoringCase("day") ||
		value.equalsIgnoringCase("daily"))
	{
		return DAILY;
	}
	else if (value.equalsIgnoringCase("week") ||
			 value.equalsIgnoringCase("weekly"))
	{
		return WEEKLY;
	}
	else if (value.equalsIgnoringCase("month") ||
			 value.equalsIgnoringCase("monthly"))
	{
		return MONTHLY;
	}
	else
	{
		error = String("Unknown time period: ") + value;
		return WEEKLY;
	}
}

void Settings::parseList(String list, vector<String>& toPopulate)
{
	toPopulate.clear();
	list.trim();

	uint32 lastEnd = 0;
	uint32 index = 0;

	while (index < list.length())
	{
		if (list.charAt(index) == ',')
		{
			if (lastEnd != index)
			{
				String value = list.subString(lastEnd, index);
				value.trim();
				toPopulate.push_back(value);
			}

			lastEnd = index + 1;
		}

		index++;
	}

	if (list.charAt(index-1) != ',')
	{
		String value = list.subString(lastEnd, index);
		value.trim();
		toPopulate.push_back(value);
	}
}

void Settings::parseExtensionList(String list, vector<String>& toPopulate, String& error)
{
	parseList(list, toPopulate);

	for (uint32 i = 0; i < toPopulate.size(); i++)
	{
		String entry = toPopulate[i];

		// If the user specifies * or *.*, just return an empty list
		// No values is interpreted as accepting anything
		if (entry.equals("*") || entry.equals("*.*"))
		{
			toPopulate.clear();
			return;
		}

		if (entry.charAt(0) == '*' && entry.charAt(1) == '.')
		{
			// If they entered something like *.cpp, strip the *
			entry = entry.subString(1);
			toPopulate[i] = entry;
		}
		
		if (entry.indexOf('*') != -1)
		{
			toPopulate.clear();
			error = "Regex expressions not supported in file extensions.\n"
				"Enter a simple list: \".cpp,.h,.java\"";
			return;
		}
	}
}
