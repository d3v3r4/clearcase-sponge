// Process.cpp

#include "Process.h"
#include <exception/IOException.h>
#include <exception/SystemException.h>
#include <util/UnixUtil.h>

#include <errno.h> // For errno
#include <fcntl.h> // For fcntl()
#include <string.h> // For strcpy()
#include <unistd.h> // For fork(), _exit()
#include <sys/types.h> // For wait_pid()
#include <sys/wait.h> // For wait_pid()

// Defines the size of buffer used when reading an entire stream
#define READ_BUFFER_SIZE 1024

Process::Process()
{
	m_hasStarted = false;
	m_hasStopped = false;
	m_return = 0;
	m_stdin = NULL;
	m_stdout = NULL;
	m_stderr = NULL;
	m_pid = -1;
}

Process::~Process()
{
	/*
	 * The zombie process that remains if we have not done a wait will have
	 * to be cleaned up by the signal handler.
	 */
	delete m_stdin;
	delete m_stdout;
	delete m_stderr;
}

void Process::execProgram(const String programName,
						  const Array<String>& args)
{
	Array<String> env(0);
	internalExec(programName, args, env, false);
}

void Process::execProgram(const String programName,
						  const Array<String>& args,
						  bool mergeOutput)
{
	Array<String> env(0);
	internalExec(programName, args, env, mergeOutput);
}

void Process::execProgram(const String programName,
						  const Array<String>& args,
						  const Array<String>& env)
{
	internalExec(programName, args, env, false);
}

void Process::execProgram(const String programName,
						  const Array<String>& args,
						  const Array<String>& env,
						  bool mergeOutput)
{
	internalExec(programName, args, env, mergeOutput);
}

void Process::execCommand(const String command)
{
	execCommand(command, false);
}

void Process::execCommand(const String command, bool mergeOutput)
{
	// Create parameter array for sh
	String quotedCommand = String('"') + command + '\"';

	Array<String> paramArray(3);
	paramArray[0] = "sh";
	paramArray[1] = "-c";
	paramArray[2] = quotedCommand;

	Array<String> envArray(0);

	internalExec("/bin/sh", paramArray, envArray, mergeOutput);
}

int32 Process::waitFor()
{
	if (!m_hasStarted)
	{
		throw SystemException("Error waiting for process completion: Process "
			"never started");
	}

	if (m_hasStopped)
	{
		return m_return;
	}

	// Wait for the process to exit. Passing zero as the last parameter means
	// we do not want waitpid to return for stopped or paused processes.
	int32 status;

	pid_t pid = waitpid(m_pid, &status, 0);

	// If the pid returned was -1 it failed for some reason
	if (pid == -1)
	{
		throw SystemException(String("Failed to wait for Process: ") +
			UnixUtil::getLastErrorMessage());
		return 1;
	}

	// If the process did not exit normally we can't get the return
	// value. For now, just returning 1 for failure.
	if (!WIFEXITED(status))
	{
		m_return = 1;
		return 1;
	}

	m_return = WEXITSTATUS(status);
	return m_return;
}

FileOutputStream* Process::getStdIn() const
{
	return m_stdin;
}

FileInputStream* Process::getStdOut() const
{
	return m_stdout;
}

FileInputStream* Process::getStdErr() const
{
	return m_stderr;
}

void Process::internalExec(const String& programName,
						   const Array<String>& args,
						   const Array<String>& env,
						   bool mergeOutput)
{
	if (m_hasStarted)
	{
		throw SystemException("Failed to create process: Cannot start two "
			"processes with one Process object");
	}

	int32 error;

	// Unix pipes (index 0 is read, index 1 is write)
	int32 stdInPipe[2] = {-1, -1};
	int32 stdOutPipe[2] = {-1, -1};
	int32 stdErrPipe[2] = {-1, -1};
	int32 childErrorPipe[2] = {-1, -1};

	// Create pipes for stdin, stdout, stderr
	if (pipe(stdInPipe) == -1)
	{
		throw SystemException(String("Failed to start. Couldn't create pipe: ") +
			UnixUtil::getLastErrorMessage());
		return;
	}

	if (pipe(stdOutPipe) == -1)
	{
		error = errno;
		closePipe(stdInPipe);
		throw SystemException(String("Failed to start. Couldn't create pipe: ") +
			UnixUtil::getErrorMessage(error));
		return;
	}

	// Only create a pipe for stderr if we need it
	if (!mergeOutput)
	{
		if (pipe(stdErrPipe) == -1)
		{
			error = errno;
			closePipe(stdInPipe);
			closePipe(stdOutPipe);
			throw SystemException(String("Failed to start. Couldn't create pipe: ") +
				UnixUtil::getErrorMessage(error));
			return;
		}
	}

	// And create one more pipe to receive errors past fork() call
	if (pipe(childErrorPipe) == -1)
	{
		error = errno;
		closePipe(stdInPipe);
		closePipe(stdOutPipe);
		closePipe(stdErrPipe);
		throw SystemException(String("Failed to start. Couldn't create pipe: ") +
			UnixUtil::getErrorMessage(error));
		return;
	}

	// Fork a new process
	pid_t pid = fork();

	if (pid == -1)
	{
		// We failed to fork for some reason
		error = errno;
		closePipe(stdInPipe);
		closePipe(stdOutPipe);
		closePipe(stdErrPipe);
		closePipe(childErrorPipe);
		throw SystemException(String("Failed to start. Couldn't fork: ") +
			UnixUtil::getErrorMessage(error));
	}
	else if (pid == 0)
	{
		// If the pid equals zero then we are the child process

		// To inform the parent process of errors we write the error number
		// to childErrorPipe. Our end will get closed by calling exec.

		// Duplicate file desciptors onto the standard io descriptors
		// and then close the unused handles

		UnixUtil::sys_dup2(stdInPipe[0], STDIN_FILENO);
		closePipe(stdInPipe);

		UnixUtil::sys_dup2(stdOutPipe[1], STDOUT_FILENO);
		closePipe(stdOutPipe);

		// Overwrite stderr with stdout if required
		if (mergeOutput)
		{
			UnixUtil::sys_dup2(STDOUT_FILENO, STDERR_FILENO);
		}
		else
		{
			UnixUtil::sys_dup2(stdErrPipe[1], STDERR_FILENO);
		}
		closePipe(stdErrPipe);

		// Make the child error pipe close on successful exec call
		if (fcntl(childErrorPipe[1], F_SETFD, FD_CLOEXEC) == -1)
		{
			error = errno;
			UnixUtil::sys_write(childErrorPipe[1], &error, sizeof(int32));
			UnixUtil::sys_close(childErrorPipe[1]);
			_exit(error);
		}

		// TODO: Should automatically add in program name if it's missing

		// Make a char** array of arguments for exec. Freed by calling exec
		// or _exit
		char** argv = allocExecArray(args);

		// If there is no enironment passed in, call execvp(), otherwise
		// create an envp array and call execve()
		if (env.size() == 0)
		{
			execvp(programName.c_str(), argv);
		}
		else
		{
			// Make a char** array of env for execve. Freed by calling exec
			// or _exit
			char** envp = allocExecArray(env);

			execve(programName.c_str(), argv, envp);
		}

		// An error occured if execvp returned.
		error = errno;
		UnixUtil::sys_write(childErrorPipe[1], &error, sizeof(int32));
		UnixUtil::sys_close(childErrorPipe[1]);
		_exit(error);
	}
	else
	{
		// The returned pid was not zero, so we are not the child process
		m_pid = pid;

		// Close unused end of childErrorPipe
		UnixUtil::sys_close(childErrorPipe[1]);

		// Try to read error from child
		ssize_t bytesRead = UnixUtil::sys_read(childErrorPipe[0], &error, sizeof(int32));

		// Close the read end of the pipe now that we don't need it either
		UnixUtil::sys_close(childErrorPipe[0]);

		// If the child process sent us an error number, throw an exception
		if (bytesRead > 0)
		{
			UnixUtil::sys_close(stdInPipe[1]);
			UnixUtil::sys_close(stdOutPipe[0]);
			UnixUtil::sys_close(stdErrPipe[0]);
			throw SystemException(String("Failed to exec process: ") +
				UnixUtil::getErrorMessage(error));
			return;
		}

		// Make stream objects
		m_stdin = new FileOutputStream(stdInPipe[1]);
		m_stdout = new FileInputStream(stdOutPipe[0]);

		if (!mergeOutput)
		{
			m_stderr = new FileInputStream(stdErrPipe[0]);
		}

		m_hasStarted = true;
	}
}

void Process::closePipe(int32* aPipe)
{
	if (aPipe[0] != -1)
	{
		UnixUtil::sys_close(aPipe[0]);
		aPipe[0] = -1;
	}
	if (aPipe[1] != -1)
	{
		UnixUtil::sys_close(aPipe[1]);
		aPipe[1] = -1;
	}
}

char** Process::allocExecArray(const Array<String>& args)
{
	// Instead of allocating a multidimensional array of null terminated
	// strings, we allocate a single array and shove both the array of char*
	// and the strings into the same buffer.
	uint32 size = sizeof(void*);

	for (uint32 i = 0; i < args.size(); i++)
	{
		size += args.get(i).length() + 1 + sizeof(void*);
	}

	char* ret = new char[size];

	// Iterate through the list of arguments, putting the address of the
	// argument at a space in the beginning of the array, and the string data
	// after the pointers at the end of the array
	char** ptrPos = (char**)ret;
	char* stringPos = ret + ((args.size()+1) * sizeof(void*));

	for (uint32 i = 0; i < args.size(); i++)
	{
		(*ptrPos) = stringPos;
		ptrPos++;

		String arg = args.get(i);
		strcpy(stringPos, arg.c_str());
		stringPos += arg.length() + 1;
	}

	(*ptrPos) = NULL;
	return (char**)ret;
}

String Process::readInputStreamLine(FileInputStream* inputStream)
{
	char buffer[1] = {0};
	int64 bytesRead = 1;
	String ret;

	while (true)
	{
		bytesRead = inputStream->read(buffer, 1);

		// Break at end of stream
		if (bytesRead < 0)
			break;

		// Eat a linefeed before a \n, but otherwise keep it
		if (buffer[0] == '\r')
		{
			inputStream->read(buffer, 1);

			if (buffer[0] == '\n')
				break;

			ret.append('\r');
			ret.append(buffer[0]);
			continue;
		}

		// Break at end of line
		if (buffer[0] == '\n')
			break;

		ret.append(buffer[0]);
	}
	return ret;
}

String Process::readAllInputStream(FileInputStream* inputStream)
{
	char buffer[READ_BUFFER_SIZE];
	int64 bytesRead = 1;
	String ret;

	while (true)
	{
		bytesRead = inputStream->read(buffer, READ_BUFFER_SIZE-1);

		// Break if there were no bytes to read
		if (bytesRead < 1)
			break;

		buffer[bytesRead] = '\0';
		ret.append(buffer);

		// It appears that, if we get less than the full buffer
		// we have hit the end of pipe
		if (bytesRead < READ_BUFFER_SIZE-1)
			break;
	}
	return ret;
}
