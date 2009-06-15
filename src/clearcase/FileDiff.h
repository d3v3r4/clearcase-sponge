// FileDiff.h

#ifndef FILE_DIFF_H
#define FILE_DIFF_H

#include <ccsponge.h>
#include <text/String.h>

/**
 * Represents the difference of two files as produced by "cleartool diff".
 */
class FileDiff
{
public:
	FileDiff(String version);
	~FileDiff();

	/*
	 * Populates with data returned from calling "cleartool diff"
	 *
	 * Throws ParsingException if the text doesn't match the expected format.
	 */
	void populate(String diffText);

	String getVersion();
	uint32 getLinesAdded();
	uint32 getLinesChanged();
	uint32 getLinesRemoved();

private:
	struct DiffRange
	{
		uint32 min;
		uint32 max;
	};

	DiffRange readRange(String diffText, uint32& index, bool& success);
	uint32 readNumber(String diffText, uint32& index, bool& success);
	void skipDiff(String diffText, uint32& index);
	void nextLine(String diffText, uint32& index);

	String m_version;
	uint32 m_linesAdded;
	uint32 m_linesChanged;
	uint32 m_linesRemoved;
};

#endif // FILE_DIFF_H
