// Condition.h

#ifndef CONDITION_H
#define CONDITION_H

#include <ccsponge.h>
#include <util/Lockable.h>

/*
 * Windows implementation of a condition for multithreaded signaling. This
 * class is somewhat complex to allow signalAll() to work correctly. Windows
 * does not have an equivilent function available except in Vista.
 *
 * If you are willing to be Vista only this class can be replaced with simple
 * usage of a CONDITION_VARIABLE and CRITICAL_SECTION.
 *
 * Current implementation requires a Windows NT based operating system (no 95/98)
 *
 * Unix-like implementation based on the article:
 * "Strategies for Implementing UNIX Condition Variables on Win32"
 * http://www.cs.wustl.edu/~schmidt/win32-cv-1.html.
 */
class Condition : public Lockable
{
public:
	/*
	 * Creates the condition and its associated mutex.
	 */
	Condition();

	/*
	 * Deletes the condition and its associated mutex.
	 *
	 * NOTE: Will put the program into a bad state if there are threads
	 * waiting on the condition.
	 */
	~Condition();

	/*
	 * Locks the mutex used by this condition. You must lock the mutex in
	 * order to either wait on the condition, or signal the condition.
	 */
	void lock();

	/*
	 * Unlocks the mutex used by this event.
	 */
	void unlock();

	/*
	 * Signals a thread waiting on this condition. This restarts one of the
	 * threads waiting on this condition.
	 */
	void signal();

	/*
	 * Signals all threads waiting on this condition. This restarts every
	 * thread waiting on this condition.
	 */
	void signalAll();

	/*
	 * Causes the calling thread to wait on the condition until signaled.
	 */
	void wait();

	/*
	 * Same as wait(), but with a timeout. Returns an estimate of the
	 * time waited. The accuracy varries by OS.
	 */
	uint32 wait(uint32 milliseconds);

private:
	Condition(const Condition& other) {}
	Condition& operator=(const Condition& other) {}

	void waitImpl(uint32 milliseconds);

private:
	// The "Mutex" for this condition. Used by lock() and unlock().
	CRITICAL_SECTION m_conditionLock;

	// Number of waiting threads
	uint32 m_waitingThreads;

	// Guards waiters_count
	CRITICAL_SECTION m_dataLock;

	// Number of threads to release via a <pthread_cond_broadcast> or a
	// <pthread_cond_signal>. 
	uint32 m_threadsToRelease;

	// Keeps track of the current "generation" so that we don't allow
	// one thread to steal all the "releases" from the broadcast.
	uint32 m_waitGenerationCount;

	// A manual-reset event that's used to block and release waiting
	// threads. 
	HANDLE m_event;
};

#endif // EVENT_H
