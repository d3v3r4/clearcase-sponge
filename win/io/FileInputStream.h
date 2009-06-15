// FileInputStream.h

#ifndef FILE_INPUT_STREAM_H
#define FILE_INPUT_STREAM_H

#include <ccsponge.h>
#include <io/InputStream.h>
#include <text/String.h>
#include <thread/Mutex.h>

/*
 * Class that wraps a file handle for reading binary data. The input is
 * unbuffered except for OS level buffering. 
 *
 * While the name matches a Java class the functionality is not quite the
 * same. The Java class is text oriented. This supports only raw binary.
 */
class FileInputStream : public InputStream
{
friend class Process;

public:
	FileInputStream();
	~FileInputStream();

	void open(const String fileName);
	void close();
	int32 read();
	int64 read(void* buffer, uint32 len);

private:
	explicit FileInputStream(HANDLE handle);
	int64 internalRead(int8* buffer, uint32 len);

	FileInputStream& operator=(const FileInputStream &other) {};
	FileInputStream(const FileInputStream& other) {};

private:
	Mutex m_mutex;
	HANDLE m_handle;
};

#endif // FILE_INPUT_STREAM_H
