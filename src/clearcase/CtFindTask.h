// CtFindTask.h

#ifndef CT_FIND_TASK_H
#define CT_FIND_TASK_H

#include <Settings.h>
#include <clearcase/AnalyzeTask.h>
#include <clearcase/DataStore.h>
#include <text/String.h>
#include <thread/ThreadPool.h>
#include <util/Runnable.h>

// The character to use when quoting parameters in command line arguments
#ifdef WINDOWS
#	define ARG_QUOTE_CHAR '\"'
#else
#	define ARG_QUOTE_CHAR '\''
#endif

/*
 * This class is a Task that will execute a "cleartool find" process.
 */
class CtFindTask : public Runnable
{
public:
	CtFindTask(ThreadPool* threadPool,
			   DataStore* dataStore,
			   Settings* settings);
	~CtFindTask();

	void run();

private:
	String makeQuery();
	String makeDirectoryList();
	String makeBranchFilter();
	String makeExcludeMainFilter();
	String makeUserFilter();
	String makeBeforeDateFilter();
	String makeAfterDateFilter();
	String makeExcludeMergesFilter();

	ThreadPool* m_threadPool;
	DataStore* m_dataStore;
	Settings* m_settings;
};

#endif // CT_FIND_TASK_H
