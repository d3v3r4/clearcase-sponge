// Thread.cpp

#include "Thread.h"

#include <exception/ThreadException.h>
#include <util/UnixUtil.h>

#include <unistd.h> // For usleep()

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
		int32 error = pthread_join(m_id, NULL);

		if (error)
		{
			cerr << "Thread::~Thread() Failed to join thread: " <<
				UnixUtil::getErrorMessage(error).c_str() << endl;
		}
	}

	delete m_runnable;
}

void Thread::start()
{
	if (m_isRunning)
	{
		throw ThreadException("Thread already started");
	}

	m_isRunning = true;

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	int32 error = pthread_create(&m_id, // Thread id
			&attr, // Attributes
			taskStarter, // Function to call
			this); // Function argument

	if (error != 0)
	{
		throw ThreadException(String("Failed to start thread: ") +
			UnixUtil::getErrorMessage(error));
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

	// Try to join the thread
	int32 error = pthread_join(m_id, NULL);

	if (error)
	{
		throw ThreadException(String("Failed to join thread: ") +
			UnixUtil::getErrorMessage(error));
	}

	// Mark as not running regardless of success
	m_isRunning = false;
}

void Thread::sleep(uint32 milliseconds)
{
	usleep(milliseconds * 1000);
}

void* Thread::taskStarter(void * arg)
{
	Thread* threadPtr = (Thread*)arg;

	try
	{
		threadPtr->run();
	}
	catch (exception& e)
	{
		cerr << "Thread::taskStarter() Unhandled exception: " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "Thread::taskStarter() Caught unknown exception" << endl;
	}

	threadPtr->m_isRunning = false;
	return NULL;
}
