// Mutex.h

#ifndef MUTEX_H
#define MUTEX_H

#include <ccsponge.h>
#include <util/Lockable.h>

/*
 * Windows mutex implementation. Wrapper around a CRITICAL_SECTION.
 */
class Mutex : public Lockable
{
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
	Mutex(const Mutex& other) {}
	Mutex& operator=(const Mutex& other) {}

private:
	CRITICAL_SECTION m_criticalSection;
};

#endif // MUTEX_H
