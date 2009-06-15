// WorkerThread.cpp

#include "WorkerThread.h"

WorkerThread::WorkerThread(ThreadPool* threadPool, Runnable* runnable)
{
	m_threadPool = threadPool;
	m_runnable = runnable;
}

WorkerThread::~WorkerThread()
{

}

void WorkerThread::run()
{
	// While the worker thread has work to do
	while (m_runnable)
	{
		m_runnable->run();
		delete m_runnable;
		m_runnable = m_threadPool->getNextTask(this);
	}

	// When getNextTask() returns null, return
}
