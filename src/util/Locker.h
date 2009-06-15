// Locker.h

#ifndef LOCKER_H
#define LOCKER_H

#include <util/Lockable.h>

/*
 * This is class intended to help in guarenteeing that an unlock call is
 * made to a class that inherits from Lockable. Normally one has to do 
 * something like the following:
 *
 * lockable.lock();
 * try {
 *   // some code
 * } catch (...) {
 *   lockable.unlock();
 *   throw;
 * }
 * lockable.unlock();
 *
 * With Locker you can rely on the constructor and destructor to call lock
 * and unlock. The destructor is called even if an exception is thrown:
 *
 * Locker locker(lockable);
 * // some code
 */
class Locker
{
public:
	Locker(Lockable& lockable);
	~Locker();

private:
	Lockable* m_lockable;
};

#endif // LOCKER_H
