// Event.cpp

#include "Event.h"


Event::Event()
{
	ErrMsg::clearLastError();

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);

	pthread_mutex_init(&m_mutex, &attr);
	pthread_cond_init(&m_cond, NULL);
}

Event::~Event()
{
	pthread_cond_destroy(&m_cond);
	pthread_mutex_destroy(&m_mutex);
}

void Event::lock()
{
	pthread_mutex_lock(&m_mutex);
}

void Event::unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

void Event::signal()
{
	pthread_cond_signal(&m_cond);
}

void Event::signalAll()
{
	pthread_cond_broadcast(&m_cond);
}

void Event::wait()
{
	pthread_cond_wait(&m_cond, &m_mutex);
}

void Event::wait(int milliseconds)
{
	long seconds = milliseconds % 1000;
	long nanoseconds = milliseconds / 1000000;

	timespec waitTillTime;
	waitTillTime.tv_sec = time(NULL) + seconds;
	waitTillTime.tv_nsec = nanoseconds;

	pthread_cond_timedwait(&m_cond, &m_mutex, &waitTillTime);
}
