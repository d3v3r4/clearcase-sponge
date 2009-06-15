// TextWriter.cpp

#include "TextWriter.h"

TextWriter::TextWriter(OutputStream* outputStream)
{
	m_outputStream = outputStream;
}

TextWriter::~TextWriter()
{

}

void TextWriter::write(String str)
{
	m_outputStream->write(str.c_str(), str.length());
}
