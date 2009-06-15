// Event.cpp

#include "Condition.h"

#include <sys/time.h>

Condition::Condition()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);

	pthread_mutex_init(&m_mutex, &attr);
	pthread_cond_init(&m_cond, NULL);
}

Condition::~Condition()
{
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
}

void Condition::lock()
{
	pthread_mutex_lock(&m_mutex);
}

void Condition::unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

void Condition::signal()
{
	pthread_cond_signal(&m_cond);
}

void Condition::signalAll()
{
	pthread_cond_broadcast(&m_cond);
}

void Condition::wait()
{
	pthread_cond_wait(&m_cond, &m_mutex);
}

uint32 Condition::wait(uint32 milliseconds)
{
	timeval startTime, endTime;
	timespec waitTillTime;

	// Note:
	// nsec = nanoseconds   1,000,000,000 of a second
	// usec = microseconds      1,000,000 of a second
	gettimeofday(&startTime, NULL);

	// pthread_cond_timedwait requires a time to wait till, not an amount of
	// time to wait, so we have to generate a time that is the current time
	// plus the passed milliseconds.
	waitTillTime.tv_sec = startTime.tv_sec + (milliseconds / 1000);
	waitTillTime.tv_nsec = (startTime.tv_usec * 1000) + ((milliseconds % 1000) * 1000000);

	// Handle nanosecond overflow
	if (waitTillTime.tv_nsec > 1000000000)
	{
		waitTillTime.tv_sec++;
		waitTillTime.tv_nsec -= 1000000000;
	}

	// Do the actual wait
	pthread_cond_timedwait(&m_cond, &m_mutex, &waitTillTime);

	// Find out how long we waited
	gettimeofday(&endTime, NULL);

	// Return the difference of endTime and startTime
	uint32 ret = (endTime.tv_sec - startTime.tv_sec) * 1000;
	ret += (endTime.tv_usec - startTime.tv_usec) / 1000;
	return ret;
}
