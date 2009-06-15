// FileOutputStream.h

#ifndef FILE_OUTPUT_STREAM_H
#define FILE_OUTPUT_STREAM_H

#include <ccsponge.h>
#include <io/OutputStream.h>
#include <text/String.h>
#include <thread/Mutex.h>

/*
 * Class that wraps a file handle for writing binary data. Output is
 * unbuffered except for OS level buffering. 
 *
 * Optionally allows a file to be openned in append mode. The appends
 * will be atomic for multiple processes unless the file is across a
 * network file system (NFS) which does not support atomic appending.
 *
 * While the name matches a Java class the functionality is not quite the
 * same. The Java class is text oriented. This supports only raw binary.
 */
class FileOutputStream : public OutputStream
{
friend class Process;

public:
	FileOutputStream();
	~FileOutputStream();

	void open(const String fileName);
	void open(const String fileName, bool append);
	void close();
	int32 write(int32 character);
	int64 write(const void* buffer, uint32 maxlen);

private:
	explicit FileOutputStream(HANDLE handle);
	void init(const String fileName, bool append);
	int64 writeNormal(const void* buffer, uint32 maxlen);
	int64 writeAppend(const void* buffer, uint32 maxlen);

	FileOutputStream& operator=(const FileOutputStream& other) {};
	FileOutputStream(const FileOutputStream& other) {};

private:
	Mutex m_mutex;
	HANDLE m_handle;
	bool m_append;
};

#endif // FILE_OUTPUT_STREAM_H
