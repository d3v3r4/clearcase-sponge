// Event.h

#ifndef EVENT_H
#define EVENT_H

#include <base.h>
#include <abstract/Lockable.h>
#include <util/ErrMsg.h>

#include <pthread.h>
#include <time.h>

/*
 * Unix implementation of an event for multithreaded signaling. Wrapper
 * around a pthread_cond_t and an associated pthread_mutex_t.
 */
class Event : public Lockable
{
public:
	/*
	 * Creates the event and its associated mutex.
	 */
	Event();

	/*
	 * Deletes the event and mutex OS object.
	 *
	 * NOTE: This WILL FAIL if there is a thread waiting on the event. As
	 *    a destructor cannot safely throw an exception this will log an
	 *    error and kill the application on failure.
	 */
	~Event();

	/*
	 *  Locks the mutex used by this event. You must lock the mutex in
	 * order to wait on the event.
	 */
	void lock();

	/*
	 *  Unlocks the mutex used by this event. You must unlock the mutex
	 * after waiting on the event.
	 */
	void unlock();

	/*
	 *  Signals a thread waiting on this event. This restarts one of the
	 * threads waiting on this event.
	 */
	void signal();

	/*
	 *  Signals all threads waiting on this event. This restarts every thread
	 * waiting on this event.
	 *
	 * NOTE: The OS scheduling policy determines the ordering in which they are
	 *    signaled and acquire the associated event mutex.
	 */
	void signalAll();

	/*
	 *  Waits on this event and unlocks the associated mutex. You must first
	 * call lock to lock the mutex. A call to wait should be followed by a call
	 * to unlock as the mutex is automatically locked once more the thread is
	 * signaled.
	 */
	void wait();

	/*
	 * Same as wait(), but with a timeout.
	 */
	void wait(int milliseconds);

private:
	pthread_cond_t m_cond;	// The pthread condition object that this class wraps
	pthread_mutex_t m_mutex;// The mutex used by the condition object
};

#endif // EVENT_H
