// FileDiff.cpp

#include "FileDiff.h"
#include <exception/ParsingException.h>

#include <iostream>
using namespace std;

FileDiff::FileDiff(String version)
{
	m_version = version;
	m_linesAdded = 0;
	m_linesChanged = 0;
	m_linesRemoved = 0;
}

FileDiff::~FileDiff()
{

}

void FileDiff::populate(String diffText)
{
	bool success = true;
	uint32 index = 0;

	// Skip warning on systems that have configuration issues
	// Can start with a string like:
	// noname: Warning: Can not find a group named "xxx\yyy"
	// noname: Warning: Can not find a group named "xxx\yyy"
	if (diffText.startsWith("noname:"))
	{
		index = diffText.indexOf('\n') + 1;

		if (diffText.indexOf("noname:", index) == index)
		{
			index = diffText.indexOf('\n', index) + 1;
		}
	}

	while (index < diffText.length() - 1)
	{
		// Read the range of lines changed in the prev copy
		DiffRange prevRange = readRange(diffText, index, success);

		if (!success)
		{
			throw ParsingException(String("Failed to parse cleartool "
				"diff text for version: ") + m_version + ", couldn't read "
				"left hand version range");
		}

		char typeChar = diffText.charAt(index);
		index++;

		// Read the range of lines changed in the current copy
		DiffRange curRange = readRange(diffText, index, success);

		if (!success)
		{
			throw ParsingException(String("Failed to parse cleartool "
				"diff text for version: ") + m_version + ", count'r read "
				"right hand version range");
		}

		// Get the number of lines changed
		uint32 prevCount = prevRange.max - prevRange.min +1;
		uint32 curCount = curRange.max - curRange.min +1;

		// Increment counters based on the diff type character
		switch (typeChar)
		{
			case 'a':
				m_linesAdded += curCount;
				break;
			case 'c':
				// We take the larger of the old and new as lengthening
				// or shortening code is both "change"
				m_linesChanged += max<uint32>(prevCount, curCount);
				break;
			case 'd':
				m_linesRemoved += prevCount;
				break;
			default:
				throw ParsingException(String("Failed to parse cleartool "
					"diff text for version: ") + m_version + ", unexpected "
					"character: \'" + typeChar + "\'");
		}

		skipDiff(diffText, index);
	}
}

uint32 FileDiff::getLinesAdded()
{
	return m_linesAdded;
}

uint32 FileDiff::getLinesChanged()
{
	return m_linesChanged;
}

uint32 FileDiff::getLinesRemoved()
{
	return m_linesRemoved;
}

FileDiff::DiffRange FileDiff::readRange(String diffText, uint32& index, bool& success)
{
	DiffRange ret;

	// Extract the first number
	ret.min = readNumber(diffText, index, success);

	if (!success)
	{
		return ret;
	}

	// If it is followed by a comma then the next number is the end of the range
	if (index < diffText.length() && diffText.charAt(index) == ',')
	{
		index++;
		ret.max = readNumber(diffText, index, success);
	}
	else // There was no comma so it is a single line change
	{
		ret.max = ret.min;
	}
	
	return ret;
}

uint32 FileDiff::readNumber(String diffText, uint32& index, bool& success)
{
	String text;

	// Extract the number from the string incrementing as we go
	while (index < diffText.length() && isdigit(diffText.charAt(index)))
	{
		text.append(diffText.charAt(index));
		index++;
	}

	return text.toUInt32(success);
}

void FileDiff::skipDiff(String diffText, uint32& index)
{
	// We should start at a newline, if not just go to the end of the line
	if (index < diffText.length() &&
		diffText.charAt(index != '\n'))
	{
		nextLine(diffText, index);
	}

	// Skip any lines that start with the special diff characters
	while (index < diffText.length() &&
		   (diffText.charAt(index) == '<' ||
		    diffText.charAt(index) == '>' ||
		    diffText.charAt(index) == '-'))
	{
		nextLine(diffText, index);
	}
}

void FileDiff::nextLine(String diffText, uint32& index)
{
	// Find the next newline
	while (index < diffText.length() &&
		   diffText.charAt(index) != '\n')
	{
		index++;
	}

	// And skip past it
	if (index < diffText.length() &&
		diffText.charAt(index) == '\n')
	{
		index++;
	}
}
