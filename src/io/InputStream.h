// InputStream.h

#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include <ccsponge.h>

/*
 * This is an abstract class representing some object from which bytes can
 * be read.
 */
class NO_VTABLE InputStream
{
public:
	/*
	 * Closes the stream.
	 */
	virtual void close() = 0;

	/*
	 * Reads the next byte of data from the input stream. Returns -1 at end of file.
	 */
	virtual int32 read() = 0;

	/*
	 * Reads up to len bytes of data from the input stream into an array of bytes.
	 * Returns -1 at end of file.
	 */
	virtual int64 read(void *buffer, uint32 len) = 0;
};

#endif // INPUT_STREAM_H
