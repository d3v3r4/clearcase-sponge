// Thread.h

#ifndef THREAD_H
#define THREAD_H

#include <ccsponge.h>
#include <util/Runnable.h>

#include <pthread.h>

/*
 * Unix thread implementation. Wrapper around pthread functions.
 *
 * You can either inherit from Thread and override the run() function or
 * you can pass in a Runnable object to the constructor.
 *
 * WARNING: The Thread object cannot go out of scope while the thread is
 * running.
 * WARNING: The Thread's run function cannot delete its own Thread object.
 * The destructor tries to join the thread. The thread cannot wait for itself.
 */
class Thread
{
public:
	/*
	 * Constucts the thread but does not start it. Use this constructor if
	 * you have inherited from thread and have overloaded run.
	 */
	Thread();

	/*
	 * This will construct a thread that runs the given Runnable. The passed
	 * Runnable must be allocated with new and will be deleted when the thread
	 * completes.
	 */
	Thread(Runnable* runnable);

	/*
	 * Calling the destructor will cause the current thread to wait for this
	 * thread to die.
	 */
    virtual ~Thread();

	/*
	 * Starts the thread
	 */
	void start();

	/*
	 * If you inherit from thread overload this to change what it does once run.
	 */
	virtual void run();

	/*
	 * Waits for the thread to finish
	 */
    void join();

	/*
	 * Causes the current thread to sleep for the specified time
	 */
	static void sleep(unsigned int milliseconds);

private:
	Thread(const Thread& other) {}
	Thread& operator=(const Thread& other) {}

private:
	/*
	 *  This is a function that simply calls the run method of the task. This
	 * is used by the constructor with a task argument.
	 */
	static void* taskStarter(void *);

	Runnable* m_runnable;	// Pointer to user specified task
	bool m_isRunning;		// set to true when the thread is started, false when joined
    pthread_t m_id;			// Thread id
};

#endif // UNIX_THREAD_H
