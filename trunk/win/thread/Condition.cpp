// Condition.cpp

#include "Condition.h"


Condition::Condition()
{
	m_waitingThreads = 0;
	m_waitGenerationCount = 0;
	m_threadsToRelease = 0;

	InitializeCriticalSection(&m_conditionLock);
	InitializeCriticalSection(&m_dataLock);

	// Create a manual-reset event.
	m_event = CreateEventW(NULL,  // Default security
						   TRUE,  // Manual-reset event
						   FALSE, // Start non-signaled
						   NULL); // Unnamed event
}

Condition::~Condition()
{
	DeleteCriticalSection(&m_conditionLock);
	DeleteCriticalSection(&m_dataLock);
	CloseHandle(m_event);
}

void Condition::lock()
{
	EnterCriticalSection(&m_conditionLock);
}

void Condition::unlock()
{
	LeaveCriticalSection(&m_conditionLock);
}

void Condition::signal()
{
	EnterCriticalSection(&m_dataLock);
	if (m_waitingThreads > m_threadsToRelease)
	{
		// Signal the manual-reset event.
		SetEvent(m_event);

		// Increment the number of threads to release by 1
		m_threadsToRelease++;

		// Start a new generation.
		m_waitGenerationCount++;
	}
	LeaveCriticalSection(&m_dataLock);
}

void Condition::signalAll()
{
	EnterCriticalSection(&m_dataLock);
	if (m_waitingThreads > 0)
	{
		// Signal the manual-reset event.
		SetEvent(m_event);

		// Release all the threads in this generation.
		m_threadsToRelease = m_waitingThreads;

		// Start a new generation.
		m_waitGenerationCount++;
	}
	LeaveCriticalSection(&m_dataLock);
}

void Condition::wait()
{
	waitImpl(INFINITE);
}

uint32 Condition::wait(uint32 milliseconds)
{
	uint32 startTime = timeGetTime();

	waitImpl(milliseconds);

	uint32 endTime = timeGetTime();
	return (endTime - startTime);
}

void Condition::waitImpl(uint32 milliseconds)
{
	EnterCriticalSection(&m_dataLock);

	// Increment count of waiters.
	m_waitingThreads++;

	// Store the current generation number.
	int myGeneration = m_waitGenerationCount;

	LeaveCriticalSection(&m_dataLock);
	LeaveCriticalSection(&m_conditionLock);

	while (true)
	{
		// Wait until the event is signaled.
		DWORD waitResult = WaitForSingleObject(m_event, milliseconds);

		EnterCriticalSection(&m_dataLock);

		// Break if there are still waiting threads from this generation
		// that haven't been released from this wait yet, or if the wait
		// times out.
		bool waitDone = ((m_threadsToRelease > 0 &&
						  m_waitGenerationCount != myGeneration) ||
						  (waitResult == WAIT_TIMEOUT));

		LeaveCriticalSection(&m_dataLock);

		if (waitDone)
			break;
	}

	// Lock m_conditionLock once more
	EnterCriticalSection(&m_conditionLock);

	// Update the information about the waiting threads
	EnterCriticalSection(&m_dataLock);
	m_waitingThreads--;
	m_threadsToRelease--;
	int last_waiter = m_threadsToRelease == 0;
	LeaveCriticalSection(&m_dataLock);

	if (last_waiter)
	{
		// We're the last waiter to be notified, so reset the manual event.
		ResetEvent(m_event);
	}
}
