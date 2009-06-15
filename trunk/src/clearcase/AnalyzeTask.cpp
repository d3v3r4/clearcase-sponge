// AnalyzeTask.cpp

#include "AnalyzeTask.h"
#include <exception/ParsingException.h>
#include <io/InputStream.h>
#include <io/TextReader.h>
#include <thread/Process.h>

#include <iostream>
#include <vector>
using namespace std;

AnalyzeTask::AnalyzeTask(ThreadPool* threadPool,
						 DataStore* dataStore,
						 Settings* settings,
						 String& versionName)
{
	m_threadPool = threadPool;
	m_dataStore = dataStore;
	m_settings = settings;
	m_versionName = versionName;
}

AnalyzeTask::~AnalyzeTask()
{

}

void AnalyzeTask::run()
{
	// Just return if the file does not pass the file extension filters
	if (!passesExtensionFilters())
	{
		return;
	}

	// Build a cleartool describe command to get the file information
	String descCommand;
	descCommand += "cleartool describe ";
	descCommand += m_versionName;

	// Execute the describe command in another process
	Process descProcess;
	descProcess.execCommand(descCommand, true);

	InputStream* stdOutStream = descProcess.getStdOut();
	TextReader descReader(stdOutStream);

	// Read all the stdout and stderr
	String descResult = descReader.readAll();

	// Parse the description into a Description object
	Description description;

	try
	{
		description.populate(m_versionName, descResult);
	}
	catch (ParsingException& e)
	{
		cout << "Error: Failed to parse 'cleartool describe' result for \""
			<< m_versionName << "\": " << e.what() << endl;
		return;
	}

	// Skip invalid versions, symbolic links and directories
	if (description.m_isInvalid ||
		description.m_isSymbolicLink ||
		description.m_isDirectory)
	{
		return;
	}

	String traceMessage = String("Analyzing: ") + m_versionName + '\n';
	cout << traceMessage;

	// Analyze other files.
	analyzeFile(description);
}

bool AnalyzeTask::passesExtensionFilters()
{
	// Ignore zero versions of files
	if (m_versionName.endsWith("\\0") ||
		m_versionName.endsWith("/0"))
	{
		return false;
	}

	// Ignore checked out files
	if (m_versionName.endsWith("CHECKEDOUT"))
	{
		cout << "2" << endl;
		return false;
	}

	// Find the index of @@ which indicates the end of the "real" file name
	int atatIndex = m_versionName.indexOf("@@");

	if (atatIndex < 0)
	{
		cout << "No @@ in version: " << m_versionName << " possible error." << endl;
		return false;
	}

	vector<String> extensions = m_settings->getExtensions();

	// If no extension parameters, accept any extension
	if (extensions.size() == 0)
	{
		return true;
	}


	// Check for a valid extension
	for (uint32 i = 0; i < extensions.size(); i++)
	{
		String extension = extensions[i];
		String filePart = m_versionName.subString(0, atatIndex);

		if (filePart.endsWith(extension))
			return true;
	}

	return false;
}

void AnalyzeTask::analyzeFile(Description& description)
{
	// Build a diff against the previous version
	// Parameters:
	// -diff_format - use unix style diff format
	// -pred - compare against previous file version
	// -blank_ignore - ignore pure white space changes
	String diffCommand("cleartool diff -diff_format -pred ");
	diffCommand.append(m_versionName);

	// Execute the diff command in another process
	Process diffProcess;
	diffProcess.execCommand(diffCommand, true);

	InputStream* stdOutStream = diffProcess.getStdOut();
	TextReader diffReader(stdOutStream);

	// Read all of stdout and stderr
	String diffResult = diffReader.readAll();

	// Don't bother with empty changes
	if (diffResult.length() == 0)
	{
		return;
	}

	// Parse the diff into a FileDiff object
	FileDiff fileDiff(m_versionName);

	try
	{
		fileDiff.populate(diffResult);
	}
	catch (ParsingException& e)
	{
		cout << "Error: Failed to parse 'cleartood diff' result for \"" << m_versionName
			<< "\" against its predecessor: " << e.what() << endl;
		return;
	}

	// Convert the ISO date to a Date object
	bool dateParsed;
	Date date = parseDate(description.m_createDate, description.m_createTime, dateParsed);

	if (!dateParsed)
	{
		cout << "Failed to convert ISO 8601 date in cleartool describe result "
			"to unix time for version: " << m_versionName << endl;
		return;
	}

	// Add the file's diff information to the data store
	m_dataStore->addData(date, fileDiff);
}

Date AnalyzeTask::parseDate(String date, String time, bool& success)
{
	uint32 year;
	uint32 month;
	uint32 day;
	uint32 hour;
	uint32 min;
	uint32 sec;

	// Verify the fixed format YYYY-MM-DD
	uint32 dash1 = date.indexOf('-');
	uint32 dash2 = date.indexOf('-', dash1+1);

	if (dash1 != 4 || dash2 != 7)
	{
		success = false;
		return Date();
	}

	// Extract substrings
	String yearPart = date.subString(0, 4);
	String monthPart = date.subString(5, 7);
	String dayPart = date.subString(8, 10);

	bool yearIsInt = true;
	bool monthIsInt = true;
	bool dayIsInt = true;
	year = yearPart.toUInt32(yearIsInt);
	month = monthPart.toUInt32(monthIsInt) - 1; // mktime expects a zero based month
	day = dayPart.toUInt32(dayIsInt);

	if (!yearIsInt ||
		!monthIsInt ||
		!dayIsInt)
	{
		success = false;
		return Date();
	}

	// Verify the fixed format HH:MM:SS-NN (NN = milliseconds)
	// Don't bother validating that the milliseconds are there
	uint32 colon1 = time.indexOf(':');
	uint32 colon2 = time.indexOf(':', colon1+1);
	//unsigned int dash = time.indexOf('-', colon2+1);

	if (colon1 != 2 || colon2 != 5 /*|| dash != 8*/)
	{
		success = false;
		return Date();
	}

	// Extract substrings
	String hourPart = time.subString(0, 2);
	String minPart = time.subString(3, 5);
	String secPart = time.subString(6, 8);

	bool hourIsInt = true;
	bool minIsInt = true;
	bool secIsInt = true;
	hour = hourPart.toUInt32(hourIsInt) - 1; // mktime expects a zero based hour
	min = minPart.toUInt32(minIsInt);
	sec = secPart.toUInt32(secIsInt);

	if (!hourIsInt ||
		!minIsInt ||
		!secIsInt)
	{
		success = false;
		return Date();
	}

	success = true;
	return Date(year, month, day, hour, min, sec);
}
