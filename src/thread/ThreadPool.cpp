// ThreadPool.cpp

#include "ThreadPool.h"
#include <exception/ThreadException.h>
#include <util/Locker.h>

ThreadPool::ThreadPool(uint32 maxThreads)
{
	init(maxThreads, 0);
}

ThreadPool::ThreadPool(uint32 maxThreads, uint32 timeoutMillis)
{
	init(maxThreads, timeoutMillis);
}

ThreadPool::ThreadPool(uint32 maxThreads, uint32 timeoutMillis, uint32 queueMax) :
	m_pending(queueMax)
{
	init(maxThreads, timeoutMillis);
}

void ThreadPool::init(uint32 maxThreads, uint32 timeoutMillis)
{
	m_maxThreads = maxThreads;
	m_timeout = timeoutMillis;
	m_threadCount = 0;
	m_waitingThreads = 0;
}

ThreadPool::~ThreadPool()
{
	shutdown();
}

void ThreadPool::execute(Runnable* runnable)
{
	m_condition.lock();

	// If the queue is stopped, shutdown was called. Throw an exception.
	if (m_pending.isStopped())
	{
		m_condition.unlock();
		throw ThreadException("Cannot execute runnable in thread pool after "
			"the pool is shut down");
	}
	
	// Join any stopped threads
	joinStopped();

	// If we are already at the limit for threads, queue the runnable.
	// If there is a maximum queue size, we have to go into a wait loop 
	// to wait for either space in the queue or the ability to start
	// another thread.
	if (m_threadCount == m_maxThreads)
	{
		bool queued = m_pending.tryPut(runnable);

		while (!queued && m_threadCount == m_maxThreads)
		{
			m_condition.wait();
			queued = m_pending.tryPut(runnable);
		}

		// If we succeeded in queueing it, return
		if (queued)
		{
			m_condition.unlock();
			return;
		}
	}

	// Otherwise increment the thread count because we will start a thread.
	m_threadCount++;
	m_condition.unlock();

	// Start a WorkerThread for the Runnable.
	WorkerThread* workerThread = new WorkerThread(this, runnable);
	workerThread->start();
}

void ThreadPool::shutdown()
{
	// Stopping the queue causes an effective shutdown
	m_pending.stop();

	Locker locker(m_condition);

	// wait until there are no running threads
	while (m_threadCount != 0)
	{
		m_condition.wait();
	}

	// Clean up and join the stopped threads
	joinStopped();
}

void ThreadPool::shutdownWhenEmpty()
{
	Locker locker(m_condition);

	// Wait till there are no remaining Runnables and all remaining threads
	// are waiting for another Runnable
	while (m_pending.size() > 0 ||
		   m_waitingThreads != m_threadCount)
	{
		m_condition.wait();
	}

	m_pending.stop();

	// Now wait until there are no running threads
	while (m_threadCount != 0)
	{
		m_condition.wait();
	}

	// Clean up and join the stopped threads
	joinStopped();
}

uint32 ThreadPool::getPendingTaskCount()
{
	return m_pending.size();
}

Runnable* ThreadPool::getNextTask(WorkerThread* caller)
{
	Runnable* ret = NULL;
	bool success;

	m_condition.lock();
	m_waitingThreads++;
	m_condition.unlock();

	// Try to get another Runnable for the given timeout
	success = m_pending.tryGet(ret, m_timeout);

	m_condition.lock();
	m_waitingThreads--;

	// Decrement the current thread count if the get failed (because the
	// thread will stop) and add the thread to the list of stopped threads.
	if (!success)
	{
		m_threadCount--;
		m_stopped.push_back(caller);
	}
	m_condition.signalAll();
	m_condition.unlock();

	return ret;
}

void ThreadPool::joinStopped()
{
	for (uint32 i = 0; i < m_stopped.size(); i++)
	{
		WorkerThread* stoppedThread = m_stopped.at(i);
		delete stoppedThread;
	}
	m_stopped.clear();
}
