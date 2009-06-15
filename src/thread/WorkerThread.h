// WorkerThread.h

#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <thread/Condition.h>
#include <thread/Thread.h>
#include <thread/ThreadPool.h>
#include <util/Runnable.h>
class ThreadPool;

/*
 * Worker thread class used by ThreadPool. Has a simple run() loop that
 * keeps asking the ThreadPool for work until the pool returns NULL.
 */
class WorkerThread : public Thread
{
public:
	WorkerThread(ThreadPool* threadPool, Runnable* runnable);
	virtual ~WorkerThread();

	virtual void run();

private:
	WorkerThread(const WorkerThread& other) {}
	WorkerThread& operator=(const WorkerThread& other) {}

private:
	ThreadPool* m_threadPool;	// Pointer to the ThreadPool that owns this
	Runnable* m_runnable;		// The current runnable being run by
};

#endif // WORKER_THREAD_H
