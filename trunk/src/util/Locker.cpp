// Locker.cpp

#include "Locker.h"

Locker::Locker(Lockable& lockable)
{
	m_lockable = &lockable;
	m_lockable->lock();
}

Locker::~Locker()
{
	m_lockable->unlock();
}
