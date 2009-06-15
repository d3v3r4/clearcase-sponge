// Mutex.cpp

#include "Mutex.h"

Mutex::Mutex()
{
	InitializeCriticalSection(&m_criticalSection);
}

Mutex::~Mutex()
{
	DeleteCriticalSection(&m_criticalSection);
}

void Mutex::lock()
{
	EnterCriticalSection(&m_criticalSection);
}

void Mutex::unlock()
{
	LeaveCriticalSection(&m_criticalSection);
}
