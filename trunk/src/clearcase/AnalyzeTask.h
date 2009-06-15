// AnalyzeTask.h

#ifndef ANALYZE_TASK_H
#define ANALYZE_TASK_H

#include <ccsponge.h>
#include <Settings.h>
#include <clearcase/DataStore.h>
#include <clearcase/Description.h>
#include <clearcase/FileDiff.h>
#include <text/String.h>
#include <thread/ThreadPool.h>
#include <util/Runnable.h>

/*
 * Runnable that checks if the given version passes the user filters and,
 * if it passes, does a diff against the version's predessesor.
 */
class AnalyzeTask : public Runnable
{
public:
	AnalyzeTask(ThreadPool* threadPool,
				DataStore* dataStore,
				Settings* settings,
				String& versionName);
	~AnalyzeTask();

	void run();

private:
	bool passesExtensionFilters();
	void analyzeFile(Description& description);
	static Date parseDate(String date, String time, bool& success);

	ThreadPool* m_threadPool;
	DataStore* m_dataStore;
	Settings* m_settings;
	String m_versionName;
};

#endif // ANALYZE_TASK_H
