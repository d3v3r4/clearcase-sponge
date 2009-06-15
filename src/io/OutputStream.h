// OutputStream.h

#ifndef OUTPUT_STREAM_H
#define OUTPUT_STREAM_H

#include <ccsponge.h>

/*
 * This is an abstract class representing some object that bytes can be
 * written to.
 */
class NO_VTABLE OutputStream
{
public:
	/*
	 * Closes the stream.
	 */
	virtual void close() = 0;

	/*
	 * Writes the given byte of data to the stream. Returns -1 on end of
	 * stream.
	 */
	virtual int32 write(int32 byte) = 0;

	/*
	 * Writes at most maxlen bytes of data from the to the stream. Returns
	 * the number of bytes written or -1 on end of stream.
	 */
	virtual int64 write(const void* buffer, uint32 maxlen) = 0;
};

#endif // OUTPUT_STREAM_H
