// Process.h

#ifndef PROCESS_H
#define PROCESS_H

#include <ccsponge.h>
#include <exception/SystemException.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <text/String.h>
#include <util/Array.h>

/*
 * Windows process implementation. Used to create child processes with
 * redirected IO.
 */
class Process
{
public:
	Process();
	~Process();

	void execProgram(const String programName,
					 const Array<String>& args);

	void execProgram(const String programName,
					 const Array<String>& args,
					 bool mergeOutput);

	void execProgram(const String programName,
					 const Array<String>& args,
					 const Array<String>& env);

	void execProgram(const String programName,
					 const Array<String>& args,
					 const Array<String>& env,
					 bool mergeOutput);

	void execCommand(const String command);
	void execCommand(const String command, bool mergeOutput);

	int32 waitFor();

	OutputStream* getStdIn() const;
	InputStream* getStdOut() const;
	InputStream* getStdErr() const;

private:
	Process(const Process& other) {}
	Process& operator=(const Process& other) {}

	void internalExec(const String command,
					  const Array<String>& args,
					  const Array<String>& env,
					  bool mergeOutput);

	static HANDLE launchProcess(const String programName,
								const Array<String>& args,
								const Array<String>& env,
								HANDLE hChildStdIn,
								HANDLE hChildStdOut,
								HANDLE hChildStdErr);

	static void closePipe(HANDLE& readHandle, HANDLE& writeHandle);
	static String makeShellPath();
	static String makeParameterString(const String command, const Array<String>& args);
	static wchar_t* allocEnvArray(const Array<String>& env);

private:
	bool m_hasStarted;
	bool m_hasStopped;
	int32 m_return;

	HANDLE m_processHandle;

	FileOutputStream* m_stdin;
	FileInputStream* m_stdout;
	FileInputStream* m_stderr;
};

#endif // PROCESS_H
