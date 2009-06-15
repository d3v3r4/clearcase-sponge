// ThreadPool.h

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread/BlockingQueue.h>
#include <thread/Mutex.h>
#include <thread/WorkerThread.h>

#include <deque>
#include <vector>
using namespace std;

/*
 * Implementation of a worker thread pool. Can be used to reduce the number
 * of times threads are started or stopped to improve performance or to
 * restrict the total number of threads running at once.
 *
 * The thread pool maintains a set of worker threads to run Runnable objects
 * passed into execute(). Calling execute() will only briefly block the
 * caller unless you set a limit to the queue size, the queue of work
 * is full and the maximum thread count was reached.
 *
 * All public functions should be thread safe.
 */
class ThreadPool
{
friend class WorkerThread;

public:
	/*
	 * Constructs a thread pool that can grow up to the given maximum
	 * number of worker threads.
	 */
	ThreadPool(uint32 maxThreads);

	/*
	 * Constructs a thread pool that can grow up to the given maximum
	 * number of worker threads. The threads will stop themselves after
	 * the given timeout if they cannot find work to do.
	 */
	ThreadPool(uint32 maxThreads, uint32 timeoutMillis);

	/*
	 * Constructs a thread pool that can grow up to the given maximum
	 * number of worker threads. The threads will stop themselves after
	 * the given timeout if they cannot find work to do.
	 *
	 * The queueMax parameter can be used to limit the growth of the
	 * queue. If the queue fills, and the thread maximum is reached,
	 * adding work to the queue will become a blocking operation.
	 */
	ThreadPool(uint32 maxThreads, uint32 timeoutMillis, uint32 queueMax);

	/*
	 * Destroys the thread pool. Equivalent to calling shutdown().
	 */
	~ThreadPool();

	/*
	 * Takes the passed Runnable and then executes it using a worker thread.
	 * The runnable passed in must be dynamically allocated using new.
	 * Will only block for an extended period if the queue is full.
	 *
	 * WARNING: Will cause access violation if the passed runnable is not
	 * allocated with new or is deleted elsewhere in the code.
	 *
	 * Throws ThreadException if shutdown() was called.
	 */
	void execute(Runnable* runnable);

	/*
	 * Returns the number of executions that are pending because of lack of
	 * threads.
	 */
	uint32 getPendingTaskCount();

	/*
	 * Cleanly causes the thread pool to shutdown and prevents new Runnables
	 * from being executed. The calling thread will block until the threads
	 * are shutdown. Be sure to only call this when you are sure no other
	 * threads will attempt to call execute().
	 */
	void shutdown();

	/*
	 * Causes the calling thread to block until the thread pool is empty
	 * and then shuts down the thread pool, blocking new calls to execute.
	 * 
	 * If the Runnable objects you pass into execute() can themselves call
	 * execute(), then you probably want to call this instead of
	 * shutdown() to make sure all the work is complete.
	 */
	void shutdownWhenEmpty();

private:
	ThreadPool(const ThreadPool& other) {}
	ThreadPool& operator=(const ThreadPool& other) {}

	/*
	 * Initializes the thread pool
	 */
	void init(uint32 maxThreads, uint32 timeoutMillis);

	/*
	 * Called by WorkerThread to get the next Runnable to work on. Returns
	 * NULL to indicate the worker needs to shut itself down.
	 */
	Runnable* getNextTask(WorkerThread* caller);

	/*
	 * This will call delete on all the threads in m_stopped and clear
	 * the vector. This causes all of the threads to join to the thread
	 * that calls this function, so be sure to call it from some other
	 * thread's stack.
	 */
	void joinStopped();

private:
	BlockingQueue<Runnable*> m_pending; // queue of executions not yet started on

	Condition m_condition; // Protects member variables, notifies when work is done
	vector<WorkerThread*> m_stopped; // List of worker threads that have finished
	uint32 m_timeout; // Timeout used by worker threads (milliseconds)
	uint32 m_maxThreads; // Maximum number of threads
	uint32 m_threadCount; // The current number of worker threads
	uint32 m_waitingThreads; // The current number of threads waiting for a Runnable
};

#endif // THREAD_POOL_H
