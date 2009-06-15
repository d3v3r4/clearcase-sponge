// Settings.h

#ifndef SETTINGS_H
#define SETTINGS_H

#include <text/String.h>
#include <util/Array.h>

#include <vector>
using namespace std;

/*
 * Holds the settings passed into the program by the user.
 */
class Settings
{
public:
	enum timePeriod
	{
		DAILY,
		WEEKLY,
		MONTHLY,
	};

	Settings();
	Settings(const Settings& other);
	~Settings();

	bool populate(Array<String>& parameters, String& error);

	bool getMergesExcluded();
	bool getMainExcluded();

	timePeriod getPeriod();

	String getOutputFile();
	String getAfterDate();
	String getBeforeDate();

	vector<String> getUsers();
	vector<String> getBrtypes();
	vector<String> getExtensions();
	vector<String> getPaths();

	Settings& operator=(const Settings& other);

private:
	timePeriod parsePeriod(String value, String& error);
	void parseList(String list, vector<String>& toPopulate);
	void parseExtensionList(String list, vector<String>& toPopulate, String& error);

private:
	bool m_excludeMerges;
	bool m_excludeMain;
	timePeriod m_period;
	String m_outputFile;
	String m_afterDate;
	String m_beforeDate;
	vector<String> m_users;
	vector<String> m_brtypes;
	vector<String> m_extensions;
	vector<String> m_paths;
};

#endif // SETTINGS_H
