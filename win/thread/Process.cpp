// Process.cpp

#include "Process.h"
#include <exception/IOException.h>
#include <exception/SystemException.h>
#include <util/WinUtil.h>

Process::Process()
{
	m_hasStarted = false;
	m_hasStopped = false;
	m_return = 0;
	m_processHandle = INVALID_HANDLE_VALUE;
	m_stdin = NULL;
	m_stdout = NULL;
	m_stderr = NULL;
}

Process::~Process()
{
	if (m_processHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_processHandle);
	}

	delete m_stdin;
	delete m_stdout;
	delete m_stderr;
}

void Process::execProgram(const String programName,
						  const Array<String>& args)
{
	Array<String> env(0);
	execProgram(programName, args, env, false);
}

void Process::execProgram(const String programName,
						  const Array<String>& args,
						  bool mergeOutput)
{
	Array<String> env(0);
	execProgram(programName, args, env, mergeOutput);
}

void Process::execProgram(const String programName,
						  const Array<String>& args,
						  const Array<String>& env)
{
	execProgram(programName, args, env, false);
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
		// Make path to cmd.exe
	String shellPath = makeShellPath();

	// Create parameter array for cmd.exe
	String quotedCommand = String('\"') + command + '\"';

	Array<String> argsArray(3);
	argsArray[0] = "cmd.exe";
	argsArray[1] = "/c";
	argsArray[2] = quotedCommand;

	Array<String> envArray(0);

	internalExec(shellPath, argsArray, envArray, mergeOutput);
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

	// Wait for the process to complete
	if (WaitForSingleObject(m_processHandle, INFINITE))
	{
		throw SystemException(String("Error waiting for process completion: ") +
			WinUtil::getLastErrorMessage());
	}

	// Get the process's return code
	DWORD returnCode;
	if (!GetExitCodeProcess(m_processHandle, &returnCode))
	{
		throw SystemException(String("Error waiting for process completion, failed "
			"to retrieve process return code: ") + WinUtil::getLastErrorMessage());
	}

	m_return = returnCode;
	m_hasStopped = true;
	return *(int*)(&returnCode);
}

OutputStream* Process::getStdIn() const
{
	return m_stdin;
}

InputStream* Process::getStdOut() const
{
	return m_stdout;
}

InputStream* Process::getStdErr() const
{
	return m_stderr;
}

void Process::internalExec(const String programName,
						   const Array<String>& args,
						   const Array<String>& env,
						   bool mergeOutput)
{
	HANDLE hInputRead = INVALID_HANDLE_VALUE;
	HANDLE hInputWrite = INVALID_HANDLE_VALUE;
	HANDLE hOutputRead = INVALID_HANDLE_VALUE;
	HANDLE hOutputWrite = INVALID_HANDLE_VALUE;
	HANDLE hErrorRead = INVALID_HANDLE_VALUE;
	HANDLE hErrorWrite = INVALID_HANDLE_VALUE;
	SECURITY_ATTRIBUTES sa;
	uint32 errorNumber;

	if (m_hasStarted)
	{
		throw SystemException("Failed to create process: Cannot start two "
			"processes with one Process object");
	}

	// Set up the security attributes struct.
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	// Create the child input pipe.
	if (!CreatePipe(&hInputRead, &hInputWrite, &sa, 0))
	{
		throw SystemException(String("Failed to create process. Couldn't create "
			"pipe: ") + WinUtil::getLastErrorMessage());
	}

	// Prevent the stdin write handle from being inherited
	if (!SetHandleInformation(hInputWrite, HANDLE_FLAG_INHERIT, 0))
	{
		errorNumber = GetLastError();
		closePipe(hInputRead, hInputWrite);
		throw SystemException(String("Failed to create process. Couldn't set "
			"handle inheritance: ") + WinUtil::getErrorMessage(errorNumber));
	}

	// Create the child output pipe.
	if (!CreatePipe(&hOutputRead, &hOutputWrite, &sa, 0))
	{
		errorNumber = GetLastError();
		closePipe(hInputRead, hInputWrite);
		throw SystemException(String("Failed to create process. Couldn't create "
			"pipe: ") + WinUtil::getErrorMessage(errorNumber));
	}

	// Prevent the stdout read handle from being inherited
	if (!SetHandleInformation(hOutputRead, HANDLE_FLAG_INHERIT, 0))
	{
		errorNumber = GetLastError();
		closePipe(hInputRead, hInputWrite);
		closePipe(hOutputRead, hOutputWrite);
		throw SystemException(String("Failed to create process. Couldn't "
			"set handle inheritance: ") + WinUtil::getErrorMessage(errorNumber));
	}

	if (!mergeOutput)
	{
		// Create the child error pipe.
		if (!CreatePipe(&hErrorRead, &hErrorWrite, &sa, 0))
		{
			errorNumber = GetLastError();
			closePipe(hInputRead, hInputWrite);
			closePipe(hOutputRead, hOutputWrite);
			throw SystemException(String("Failed to create process. Couldn't "
				"create pipe: ") + WinUtil::getErrorMessage(errorNumber));
		}

		// Prevent the stderr read handle from being inherited
		if (!SetHandleInformation(hErrorRead, HANDLE_FLAG_INHERIT, 0))
		{
			errorNumber = GetLastError();
			closePipe(hInputRead, hInputWrite);
			closePipe(hOutputRead, hOutputWrite);
			closePipe(hErrorRead, hErrorWrite);
			throw SystemException(String("Failed to create process. Couldn't "
				"set handle inheritance: ") + WinUtil::getErrorMessage(errorNumber));
		}
	}
	else
	{
		// If we are merging the output, just duplicate the stdout write
		// handle and make the child use it as its stderr

		if (!DuplicateHandle(GetCurrentProcess(), // Source Process
							 hOutputWrite, // Source HANDLE
							 GetCurrentProcess(), // Destination Process
							 &hErrorWrite, // Destination HANDLE address
							 0, // Access rights (ignored due to DUPLICATE_SAME_ACCESS)
							 TRUE, // Make the HANDLE inheritable
							 DUPLICATE_SAME_ACCESS)) // Use same access rights
		{
			closePipe(hInputRead, hInputWrite);
			closePipe(hOutputRead, hOutputWrite);
			throw SystemException(String("Failed to create process. Couldn't "
				"duplicate handle: ") + WinUtil::getLastErrorMessage());
		}
	}

	// Start the process
	m_processHandle = launchProcess(programName, args, env, hInputRead, hOutputWrite, hErrorWrite);

	// Close our copies of the pipe handles that the child process will use.
	CloseHandle(hInputRead);
	CloseHandle(hOutputWrite);
	CloseHandle(hErrorWrite);

	// Make stream objects
	m_stdin = new FileOutputStream(hInputWrite);
	m_stdout = new FileInputStream(hOutputRead);

	if (!mergeOutput)
	{
		m_stderr = new FileInputStream(hErrorRead);
	}

	m_hasStarted = true;
}

HANDLE Process::launchProcess(const String programName,
							  const Array<String>& args,
							  const Array<String>& env,
							  HANDLE hChildStdIn,
							  HANDLE hChildStdOut,
							  HANDLE hChildStdErr)
{
	String parameterString = makeParameterString(programName, args);

	// Create the needed PROCESS_INFORMATION and STARTUPINFO structs
	PROCESS_INFORMATION processInfo;
	STARTUPINFOA startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
    ZeroMemory(&processInfo, sizeof(processInfo));

	// Set up the start up info struct.
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwX = CW_USEDEFAULT;
	startupInfo.dwY = CW_USEDEFAULT;
	startupInfo.dwXSize = CW_USEDEFAULT;
	startupInfo.dwYSize = CW_USEDEFAULT;
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdOutput = hChildStdOut;
	startupInfo.hStdInput  = hChildStdIn;
	startupInfo.hStdError  = hChildStdErr;

	// Allocate an array containing the environment variables in the desired
	// format. Returns NULL in env.size() == 0. Passing NULL into
	// CreateProcess will use the default environment as desired.
	wchar_t* envArray = allocEnvArray(env);

	// Launch the process that you want to redirect (in this case,
	// Child.exe). Make sure Child.exe is in the same directory as
	// redirect.c launch redirect from a command line to prevent location
	// confusion.
	if (!CreateProcessA(programName.c_str(), // Name of the executable
					   (LPSTR)parameterString.c_str(), // Parameters of the executable
					   NULL, // Default process attributes
					   NULL, // Default thread attributes
					   TRUE, // Use the std handles
					   CREATE_DEFAULT_ERROR_MODE | CREATE_UNICODE_ENVIRONMENT, // Don't inherit error mode, the env is unicode
					   envArray, // Environment variables
					   NULL, // Run in current directory
					   &startupInfo, // Pass in our startup information
					   &processInfo)) // CreateProcess will fill this PROCESS_INFORMATION struct
	{
		delete[] envArray;
		throw SystemException(String("Failed to create process: ") +
			WinUtil::getLastErrorMessage());
	}

	delete[] envArray;

	// Close the unneeded handle to the process's primary thread
	CloseHandle(processInfo.hThread);

	// Return the process's handle
	return processInfo.hProcess;
}

void Process::closePipe(HANDLE& readHandle, HANDLE& writeHandle)
{
	if (readHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(readHandle);
		readHandle = INVALID_HANDLE_VALUE;
	}
	if (writeHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(writeHandle);
		writeHandle = INVALID_HANDLE_VALUE;
	}
}

String Process::makeShellPath()
{
	// Ask how long the path to the system folder is
	uint32 pathLen = GetSystemDirectory(NULL, 0);
	if (pathLen == 0)
	{
		throw SystemException(String("Failed to start process. Couldn't "
			"retrieve path to \"cmd.exe\"") + WinUtil::getLastErrorMessage());
	}

	// Allocate the buffer on the stack with room for the added "\\cmd.exe"
	char* cmdPathBuffer = (char*)_alloca(pathLen + 8);

	// Fill the buffer with the real path
	pathLen = GetSystemDirectory(cmdPathBuffer, pathLen + 8);
	if (pathLen == 0)
	{
		throw SystemException(String("Failed to start process. Couldn't "
			"retrieve path to \"cmd.exe\"") + WinUtil::getLastErrorMessage());
	}

	// Append the name of the command interpreter
	strcat(cmdPathBuffer, "\\cmd.exe");

	// Make string object to return
	return String(cmdPathBuffer);
}

String Process::makeParameterString(const String command, const Array<String>& args)
{
	// TODO: Check if arguments includes the program name?

	String ret;

	for (uint32 i = 0; i < args.size(); i++)
	{
		ret.append(args.get(i) + " ");
	}

	return ret;
}

wchar_t* Process::allocEnvArray(const Array<String>& env)
{
	if (env.size() == 0)
		return NULL;

	// CreateProcess wants an unusual format of an array containing null
	// terminated strings, terminated by two nulls. For example:
	//
	// PATH=C:\\0PIE=APPLE\0PS1='pwd'\0\0\0
	//
	// This function returns unicode characters because CreateProcess has
	// this requirement: "If this parameter is NULL and the environment
	// block of the parent process contains Unicode characters, you must
	// also ensure that dwCreationFlags includes CREATE_UNICODE_ENVIRONMENT."
	//
	// It's easier to have this generate unicode than to check if the
	// environment of this process contains unicode. Should do proper
	// conversion after adding unicode support.
	
	uint32 arraySize = 2;

	for (uint32 i = 0; i < env.size(); i++)
	{
		arraySize += env.get(i).length() + 1;
	}

	wchar_t* ret = new wchar_t[arraySize];
	uint32 pos = 0;

	for (uint32 i = 0; i < env.size(); i++)
	{
		String envValue = env.get(i);

		// Convert the characters to wide characters as we copy them in
		for (uint32 j = 0; j < envValue.length(); j++)
		{
			ret[pos+j] = envValue.charAt(j);
		}
		ret[pos+envValue.length()] = 0;

		pos += env.get(i).length() + 1;
	}

	// Appending trailing '\0' character
	ret[pos] = 0;

	return ret;
}
