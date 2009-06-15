// CtFindTask.cpp

#include "CtFindTask.h"
#include <io/InputStream.h>
#include <io/TextReader.h>
#include <thread/Process.h>
#include <util/Array.h>

#include <iostream>
#include <vector>
using namespace std;

CtFindTask::CtFindTask(ThreadPool* threadPool,
					   DataStore* dataStore,
					   Settings* settings)
{
	m_threadPool = threadPool;
	m_dataStore = dataStore;
	m_settings = settings;
}

CtFindTask::~CtFindTask()
{

}

void CtFindTask::run()
{
	// Build command string
	String command = makeQuery();

	// Execult the query in another process
	Process findProcess;
	findProcess.execCommand(command, true);

	InputStream* stdOutStream = findProcess.getStdOut();
	TextReader findReader(stdOutStream);
	bool readSuccess;

	// Extract the first returned version name
	String versionName = findReader.readLine(readSuccess);

	// If the returned string is empty just return as we found nothing
	if (!readSuccess)
	{
		return;
	}

	// Skip warning on systems that have configuration issues
	// Can start with a string like:
	// noname: Warning: Can not find a group named "xxx\yyy"
	if (versionName.startsWith("noname: Warning:"))
	{
		versionName = findReader.readLine(readSuccess);

		if (!readSuccess)
		{
			return;
		}
	}

	// Print errors returned from cleartool
	if (versionName.startsWith("cleartool: Error:"))
	{
		cerr << "Failed to execute cleartool find command. Aborting." << endl
			<< versionName << endl;
		exit(1);
	}

	AnalyzeTask* analyzeTask = new AnalyzeTask(m_threadPool,
			m_dataStore,
			m_settings,
			versionName);
	m_threadPool->execute(analyzeTask);

	// Loop to analyze remaining versions
	while (true)
	{
		versionName = findReader.readLine(readSuccess);

		if (!readSuccess)
			break;

		analyzeTask = new AnalyzeTask(m_threadPool,
			m_dataStore,
			m_settings,
			versionName);
		m_threadPool->execute(analyzeTask);
	}

	// Wait for the find process to exit
	findProcess.waitFor();
}

String CtFindTask::makeQuery()
{
	vector<String> filters;

	// Make the assorted filters, ignoreing ones that return an
	// empty String object as it indicates "no filter".
	String branchFilter = makeBranchFilter();
	if (branchFilter.length() > 0)
		filters.push_back(branchFilter);

	String excludeMainFilter = makeExcludeMainFilter();
	if (excludeMainFilter.length() > 0)
		filters.push_back(excludeMainFilter);

	String userFilter = makeUserFilter();
	if (userFilter.length() > 0)
		filters.push_back(userFilter);

	String beforeDateFilter = makeBeforeDateFilter();
	if (beforeDateFilter.length() > 0)
		filters.push_back(beforeDateFilter);

	String afterDateFilter = makeAfterDateFilter();
	if (afterDateFilter.length() > 0)
		filters.push_back(afterDateFilter);

	String excludeMergesFilter = makeExcludeMergesFilter();
	if (excludeMergesFilter.length() > 0)
		filters.push_back(excludeMergesFilter);

	// Build command string
	String command("cleartool find ");
	command.append(makeDirectoryList());
	command.append(" -version ");
	command.append(ARG_QUOTE_CHAR);

	String versionFilter;

	// And each of the filters together
	for (uint32 i = 0; i < filters.size(); i++)
	{
		versionFilter.append(filters.at(i));

		if (i != filters.size() - 1)
		{
			versionFilter.append(" && ");
		}
	}

	// If the version filter is empty, create a dummy filter that finds
	// anything. Cleartool won't accept an empty filter.
	if (versionFilter.length() == 0)
	{
		versionFilter.assign("lbtype(X) || !lbtype(X)");
	}

	command.append(versionFilter);
	command.append(ARG_QUOTE_CHAR);
	command.append(" -print");

	return command;
}

String CtFindTask::makeDirectoryList()
{
	vector<String> pathsArgs = m_settings->getPaths();
	String ret;

	for (uint32 i = 0; i < pathsArgs.size(); i++)
	{
		ret.append(pathsArgs[i]);
		if (i != pathsArgs.size()-1)
			ret.append(" ");
	}

	return ret;
}

String CtFindTask::makeBranchFilter()
{
	vector<String> branchArgs = m_settings->getBrtypes();
	String ret;

	if (branchArgs.size() == 0)
		return ret;

	ret.append('(');

	// Make a list of brtype filteres or-ed together
	for (uint32 i = 0; i < branchArgs.size(); i++)
	{
		ret.append("brtype(");
		ret.append(branchArgs[i]);
		ret.append(") ");

		if (i != branchArgs.size() - 1)
		{
			ret.append("|| ");
		}
	}
	ret.append(')');

	return ret;
}

String CtFindTask::makeExcludeMainFilter()
{
	bool excludeMain = m_settings->getMainExcluded();

	if (excludeMain)
	{
		return String("(!brtype(main))");
	}
	else
	{
		return String();
	}
}

String CtFindTask::makeUserFilter()
{
	vector<String> userArgs = m_settings->getUsers();
	String ret;

	if (userArgs.size() == 0)
		return ret;

	ret.append('(');

	// Make a list of created_by filteres or-ed together
	for (uint32 i = 0; i < userArgs.size(); i++)
	{
		ret.append("created_by(");
		ret.append(userArgs[i]);
		ret.append(") ");

		if (i != userArgs.size() - 1)
		{
			ret.append("|| ");
		}
	}
	ret.append(')');

	return ret;
}

String CtFindTask::makeBeforeDateFilter()
{
	String beforeArg = m_settings->getBeforeDate();
	String ret;

	if (beforeArg.length() == 0)
		return ret;

	// We use ! with the created_since filter to do a "before" filter
	ret.append("(!created_since(");
	ret.append(beforeArg);
	ret.append("))");

	return ret;
}

String CtFindTask::makeAfterDateFilter()
{
	String afterArg = m_settings->getAfterDate();
	String ret;

	if (afterArg.length() == 0)
		return ret;

	ret.append("(created_since(");
	ret.append(afterArg);
	ret.append("))");

	return ret;
}

String CtFindTask::makeExcludeMergesFilter()
{
	bool excludeMerges = m_settings->getMergesExcluded();
	String ret;

	if (!excludeMerges)
		return ret;

	// hltype means "hyperlink type". This will filter out any file with
	// a merge arrow pointing to the file.
	ret.append("(!hltype(Merge, ->))");

	return ret;
}
