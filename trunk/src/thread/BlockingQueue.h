// BlockingQueue.h

#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H

#include <ccsponge.h>
#include <thread/Condition.h>
#include <util/Locker.h>

#include <deque>
using namespace std;

/*
 * A thread safe queue that can be safely accessed by multiple threads.
 * Intended to be used as part of a producer/consumer pattern.
 *
 * To ease implementation of such an algorithm, there is a function called
 * stop() which will prevent new items from being added to the queue and
 * cause all gets to return false once the queue is empty. This is frequently
 * needed so that consumers do not wait forever for more input.
 */
template <typename T>
class BlockingQueue
{
public:
	BlockingQueue();
	BlockingQueue(uint32 max);
	~BlockingQueue();

	/*
	 * Adds an element to the queue. If not stopped, will block till there
	 * is room to add and always return true. If stopped, will always return
	 * false.
	 */
	bool put(T element);

	/*
	 * Removes an element from the queue. If not stopped, will block till
	 * there an element to get and will always return true. If stopped and
	 * an element is available, will get the element and return true. If
	 * stopped and an element is not available, will always return false.
	 */
	bool get(T& element);

	/*
	 * If not stopped and room for the element in the queue, will add the
	 * element to the queue and return true. Will do nothing and return
	 * false if stopped or if there is no room in the queue.
	 */
	bool tryPut(T element);

	/*
	 * If an element is in the queue, gets the element and returns true.
	 * Returns false if nothing is in the queue.
	 */
	bool tryGet(T& element);

	/*
	 * If an element is in the queue, gets the element and returns true.
	 * If an element is not currently in the queue, it will block for up
	 * to the passed time for an element to be added to the queue. Returns
	 * false if an element is never retrieved.
	 *
	 * The timeout is ignored if the queue is stopped and it will return
	 * false immediately.
	 */
	bool tryGet(T& element, uint32 milliseconds);

	/*
	 * Stops the queue. This does two things:
	 *
	 * 1) Elements can no longer be added to the queue. Put functions
	 * return false.
	 * 2) Get functions will return false when the queue is empty.
	 */
	void stop();

	/*
	 * Returns true if stop() has been called, false if not.
	 */
	bool isStopped();

	/*
	 * Returns the current size of the queue.
	 */
	uint32 size();

	/*
	 * Assignment operator.
	 */
	BlockingQueue& BlockingQueue::operator=(const BlockingQueue& other);

private:
	mutable Condition m_condition;
	deque<T> m_deque;
	uint32 m_maxSize;
	bool m_stopped;
};

template <typename T>
BlockingQueue<T>::BlockingQueue()
{
	m_stopped = false;
	m_maxSize = 0xffff;
}

template <typename T>
BlockingQueue<T>::BlockingQueue(uint32 max)
{
	m_stopped = false;
	m_maxSize = max;
}

template <typename T>
BlockingQueue<T>::~BlockingQueue()
{

}

template <typename T>
bool BlockingQueue<T>::put(T element)
{
	Locker locker(m_condition);

	if (m_stopped)
		return false;

	// Wait until there is space
	while (m_deque.size() >= m_maxSize)
	{
		if (m_stopped)
			return false;

		m_condition.wait();
	}

	// Add the element
	m_deque.push_back(element);

	// Notify waiting threads
	m_condition.signalAll();
	return true;
}

template <typename T>
bool BlockingQueue<T>::get(T& element)
{
	Locker locker(m_condition);

	// Wait until there is something to get
	while (m_deque.size() == 0)
	{
		if (m_stopped)
			return false;

		m_condition.wait();
	}

	// Fetch the element from the deque
	element = m_deque.front();
	m_deque.pop_front();

	// Notify waiting threads
	m_condition.signalAll();
	return true;
}

template <typename T>
bool BlockingQueue<T>::tryPut(T element)
{
	Locker locker(m_condition);

	// Return false if the queue is stopped or there is no space to add
	// the element
	if (m_stopped ||
		m_deque.size() >= m_maxSize)
	{
		return false;
	}

	// Add the element
	m_deque.push_back(element);

	// Notify waiting threads
	m_condition.signalAll();
	return true;
}

template <typename T>
bool BlockingQueue<T>::tryGet(T& element)
{
	Locker locker(m_condition);

	// Return false if the queue is empty
	if (m_deque.size() == 0)
	{
		return false;
	}

	// Fetch the element from the deque
	element = m_deque.front();
	m_deque.pop_front();

	// Notify waiting threads
	m_condition.signalAll();
	return true;
}

template <typename T>
bool BlockingQueue<T>::tryGet(T& element, uint32 milliseconds)
{
	Locker locker(m_condition);

	// Essential idea is that we accumulate the time spent waiting and return
	// false if the time is exceeded. Also returns false immediately if stop()
	// is called.
	uint32 totalWait = 0;

	while (m_deque.size() == 0)
	{
		if (m_stopped ||
			totalWait >= milliseconds)
		{
			return false;
		}

		totalWait += m_condition.wait(milliseconds - totalWait);
	}

	element = m_deque.front();
	m_deque.pop_front();
	return true;
}

template <typename T>
void BlockingQueue<T>::stop()
{
	Locker locker(m_condition);
	m_stopped = true;
	m_condition.signalAll();
}

template <typename T>
bool BlockingQueue<T>::isStopped()
{
	Locker locker(m_condition);
	return m_stopped;
}

template <typename T>
uint32 BlockingQueue<T>::size()
{
	Locker locker(m_condition);
	return m_deque.size();
}

template <typename T>
BlockingQueue<T>& BlockingQueue<T>::operator=(const BlockingQueue<T>& other)
{
	if (&other == this)
	{
		return *this;
	}

	Locker myLocker(m_condition);
	Locker otherLocker(other.m_condition);

	m_deque = other.m_deque;
	m_maxSize = other.m_maxSize;
	m_stopped = other.m_stopped;
	return *this;
}

#endif // BLOCKING_QUEUE_H
