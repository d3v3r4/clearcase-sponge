// Condition.h

#ifndef CONDITION_H
#define CONDITION_H

#include <ccsponge.h>
#include <util/Lockable.h>

#include <pthread.h>

/*
 * Unix implementation of an event for multithreaded signaling. Wrapper
 * around a pthread_cond_t and an associated pthread_mutex_t.
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
	 * Same as wait(), but with a timeout.
	 */
	uint32 wait(uint32 milliseconds);

private:
	Condition(const Condition& other) {}
	Condition& operator=(const Condition& other) {}

private:
	pthread_cond_t m_cond;	// The pthread condition object that this class wraps
	pthread_mutex_t m_mutex;// The mutex used by the condition object
};

#endif // CONDITION_H
