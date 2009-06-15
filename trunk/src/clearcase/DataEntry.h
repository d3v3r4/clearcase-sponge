// DataEntry.h

#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H

#include <ccsponge.h>
#include <clearcase/FileDiff.h>
#include <text/String.h>

/*
 * An entry in a DataStore object. Contains the lines added, changed and
 * removed.
 */
class DataEntry
{
public:
	DataEntry();
	DataEntry(FileDiff& fileDiff);
	~DataEntry();

	void add(FileDiff& fileDiff);
	void add(DataEntry& otherEntry);

	uint32 getLinesAdded();
	uint32 getLinesChanged();
	uint32 getLinesRemoved();

private:
	uint32 m_linesAdded;
	uint32 m_linesChanged;
	uint32 m_linesRemoved;
};

#endif // DATA_ENTRY_H
