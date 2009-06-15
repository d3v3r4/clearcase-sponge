// Thread.h

#ifndef THREAD_H
#define THREAD_H

#include <ccsponge.h>
#include <util/Runnable.h>

/*
 * Windows thread implementation. Wrapper around a Windows thread.
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
	 * thread to join (if it hasn't already).
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
	static void sleep(uint32 milliseconds);

private:
	Thread(const Thread& other) {}
	Thread& operator=(const Thread& other) {}

	/*
	 * Functions used to call the run method and handle exceptions.
	 */
	static unsigned __stdcall taskStarter1(void*);
	static void taskStarter2(Thread*);

private:
	Runnable* m_runnable;	// pointer to Runnable to run
	bool m_isRunning;		// set to true when the thread is started, false when joined
    HANDLE m_handle;		// handle to thread
    unsigned int m_id;		// thread id
};

#endif // THREAD_H
