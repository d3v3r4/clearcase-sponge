// Thread.cpp

#include "Thread.h"
#include <exception/ThreadException.h>
#include <util/WinUtil.h>

#include <process.h>

#include <iostream>
using namespace std;

Thread::Thread()
{
	m_runnable = NULL;
	m_isRunning = false;
}

Thread::Thread(Runnable* runnable)
{
	m_runnable = runnable;
	m_isRunning = false;
}

Thread::~Thread()
{
	// If the thread is running have it join
	if (m_isRunning)
	{
		DWORD ret = WaitForSingleObject(m_handle, INFINITE);

		if (ret != WAIT_OBJECT_0)
		{
			cerr << "Thread::~Thread() Failed to join thread: " <<
				WinUtil::getLastErrorMessage() << endl;
		}
	}

	// Close the handle to the thread
	CloseHandle(m_handle);

	// Delete the runnable
	delete m_runnable;
}

void Thread::start()
{
	if (m_isRunning)
	{
		throw ThreadException("Thread already started");
	}

	m_isRunning = true;

	// Using _beginthreadex() because CreateThread has a documented memory
	// leak with the CRT due to use of thread local variables.
	m_handle = (HANDLE)_beginthreadex(NULL, // Default security attributes
		0,				// Default stack size
		taskStarter1,	// Method to call
		(void*)this,	// Method arguement
        0,				// Start immediately
        &m_id);			// pointer to id (will be set)

	if (m_handle == NULL)
	{
		throw ThreadException(String("Failed to start thread: ") +
			WinUtil::getLastErrorMessage());
	}
}

void Thread::run()
{
	if (m_runnable != NULL)
		m_runnable->run();
}

void Thread::join()
{
	if (!m_isRunning)
	{
		throw ThreadException("Failed to join thread: Thread not running");
	}

	DWORD ret = WaitForSingleObject(m_handle, INFINITE);

	if (ret != WAIT_OBJECT_0)
	{
		throw ThreadException(String("Failed to join thread: ") +
			WinUtil::getLastErrorMessage());
	}

	m_isRunning = false;
}

void Thread::sleep(uint32 milliseconds)
{
	Sleep(milliseconds);
}

unsigned __stdcall Thread::taskStarter1(void* arg)
{
	Thread* threadPtr = (Thread*)arg;

	// Catch any c++ exceptions that were not caught by the structured
	// exception handling in taskStarter2()
	try
	{
		taskStarter2(threadPtr);
	}
	catch (exception& e)
	{
		cerr << "Thread::taskStarter() Unhandled exception: " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "Thread::taskStarter() Unhandled unknown exception" << endl;
	}

	threadPtr->m_isRunning = false;
	return NULL;
}

void Thread::taskStarter2(Thread* threadPtr)
{
	// Some structured exception handling to get slightly better logging of
	// a few common problems
	__try
	{
		threadPtr->run();
	}
	__except((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) ||
			 (GetExceptionCode() == EXCEPTION_INT_DIVIDE_BY_ZERO) ||
			 (GetExceptionCode() == EXCEPTION_FLT_DIVIDE_BY_ZERO) ||
			 (GetExceptionCode() == EXCEPTION_INVALID_HANDLE) ||
			 (GetExceptionCode() == EXCEPTION_STACK_OVERFLOW))
	{
		uint32 code = GetExceptionCode();

		switch (code)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			cerr << "Thread::taskStarter() Uncaught exception: Access violation" << endl;
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			cerr << "Thread::taskStarter() Uncaught exception: Integer devide by zero" << endl;
			break;
		case EXCEPTION_INVALID_HANDLE:
			cerr << "Thread::taskStarter() Uncaught exception: Invalid handle used" << endl;
			break;
		case EXCEPTION_STACK_OVERFLOW:
			//_resetstkoflw(); // Restore page guard for the stack (Not needed)
			cerr << "Thread::taskStarter() Uncaught exception: Stack overflow" << endl;
			break;
		}
	}
}