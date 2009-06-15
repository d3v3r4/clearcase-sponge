// TextReader.cpp

#include "TextReader.h"


TextReader::TextReader(InputStream* inputStream)
{
	m_inputStream = inputStream;
}

TextReader::~TextReader()
{

}

String TextReader::readLine(bool& success)
{
	char buffer[1] = {0};
	int64 bytesRead = 1;
	String ret;

	success = false;

	while (true)
	{
		bytesRead = m_inputStream->read(buffer, 1);

		// Break at end of stream
		if (bytesRead < 0)
			break;

		success = true;

		// Eat a linefeed before a \n, but otherwise keep it
		if (buffer[0] == '\r')
		{
			m_inputStream->read(buffer, 1);
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

String TextReader::readAll()
{
	char buffer[1024];
	int64 bytesRead = 1;
	String ret;

	while (true)
	{
		bytesRead = m_inputStream->read(buffer, 1023);

		if (bytesRead < 0)
			break;

		buffer[bytesRead] = '\0';
		ret.append(buffer);
	}

	ret.replaceAll("\r\n", "\n");
	return ret;
}
