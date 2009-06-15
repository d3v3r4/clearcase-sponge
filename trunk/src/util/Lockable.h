// Lockable.h

#ifndef LOCKABLE_H
#define LOCKABLE_H

#include <ccsponge.h>

/*
 * This is a abstract class that defines a class that can be somehow locked
 * and unlocked by the Locker class.
 */
class NO_VTABLE Lockable
{
public:
	virtual void lock() = 0;
	virtual void unlock() = 0;
};

#endif // LOCKABLE_H
