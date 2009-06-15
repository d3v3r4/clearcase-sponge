// Mutex.h

#ifndef MUTEX_H
#define MUTEX_H

#include <ccsponge.h>
#include <util/Lockable.h>

#include <pthread.h>

/*
 * The Unix Mutex class is a wrapper around a pthread_mutex_t object.
 */
class Mutex : public Lockable
{
public:
	Mutex(void);
	~Mutex(void);

	void lock();
	void unlock();

private:
	Mutex(const Mutex& other) {}
	Mutex& operator=(const Mutex& other) {}

private:
	pthread_mutex_t m_mutex;
};

#endif // MUTEX_H
