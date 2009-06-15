// FileInputStream.h

#ifndef FILE_INPUT_STREAM_H
#define FILE_INPUT_STREAM_H

#include <ccsponge.h>
#include <io/InputStream.h>
#include <thread/Mutex.h>
#include <text/String.h>

/*
 * Class that wraps a file handle for reading binary data. The input is
 * unbuffered except for OS level buffering. While the name matches a Java
 * class the functionality is not quite the same. The Java class is text
 * oriented, this reads raw bytes.
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
	explicit FileInputStream(int32 fileDescriptor);
	int64 internalRead(void* buffer, uint32 len);

	FileInputStream(const FileInputStream& other) {};
	FileInputStream& operator=(const FileInputStream &other) {};

	Mutex m_mutex;
	int32 m_fileDescriptor;
};

#endif // FILE_INPUT_STREAM_H
