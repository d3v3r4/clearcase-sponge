// TextReader.h

#ifndef TEXT_READER_H
#define TEXT_READER_H

#include <io/InputStream.h>
#include <text/String.h>

/*
 * For now this class is a simplistic wrapper around an InputStream. It's
 * basically a stub to add conversion from some other encoding to unicode,
 * if unicode support is added.
 */
class TextReader
{
public:
	TextReader(InputStream* inputStream);
	~TextReader();

	/*
	 * Reads the next line of text from the stream.
	 *
	 * TODO: Really need a null String value for functions like this
	 */
	String readLine(bool& success);

	/*
	 * Reads all of the text from the stream.
	 *
	 * TODO: Really need a null String value for functions like this
	 */
	String readAll();

private:
	InputStream* m_inputStream;
};

#endif // TEXT_READER_H
