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
 * Unix Process implementation. Allows for executing both programs and shell
 * commands. Shell commands interpreted with the Bourne shell (or whatever
 * is named "/bin/sh")
 *
 * Processes not waited for will leak as zombie processes. Failure to read
 * process output can result in deadlock or corruption when the output buffer
 * space is limited by the OS.
 *
 * Not safe for access by multiple threads.
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

	void writeStdIn(String string);

	String readStdOutLine();
	String readStdErrLine();

	String readAllStdOut();
	String readAllStdErr();

private:
	Process(const Process& other) {}
	Process& operator=(const Process& other) {}

	void internalExec(const String& program,
					  const Array<String>& args,
					  const Array<String>& env,
					  bool mergeOutput);

	static void closePipe(int32* aPipe);
	static char** allocExecArray(const Array<String>& args);
	static String readInputStreamLine(FileInputStream* inputStream);
	static String readAllInputStream(FileInputStream* inputStream);

private:
	bool m_hasStarted;
	bool m_hasStopped;
	int32 m_return;

	pid_t m_pid;

	FileOutputStream* m_stdin;
	FileInputStream* m_stdout;
	FileInputStream* m_stderr;
};

#endif // UNIX_PROCESS_H
