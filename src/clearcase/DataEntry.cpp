// DataEntry.cpp

#include "DataEntry.h"


DataEntry::DataEntry()
{
	m_linesAdded = 0;
	m_linesChanged = 0;
	m_linesRemoved = 0;
}

DataEntry::DataEntry(FileDiff& fileDiff)
{
	m_linesAdded = fileDiff.getLinesAdded();
	m_linesChanged = fileDiff.getLinesChanged();
	m_linesRemoved = fileDiff.getLinesRemoved();
}

DataEntry::~DataEntry()
{

}

void DataEntry::add(FileDiff& fileDiff)
{
	m_linesAdded += fileDiff.getLinesAdded();
	m_linesChanged += fileDiff.getLinesChanged();
	m_linesRemoved += fileDiff.getLinesRemoved();
}

void DataEntry::add(DataEntry& otherEntry)
{
	m_linesAdded += otherEntry.m_linesAdded;
	m_linesChanged += otherEntry.m_linesChanged;
	m_linesRemoved += otherEntry.m_linesRemoved;
}

uint32 DataEntry::getLinesAdded()
{
	return m_linesAdded;
}

uint32 DataEntry::getLinesChanged()
{
	return m_linesChanged;
}

uint32 DataEntry::getLinesRemoved()
{
	return m_linesRemoved;
}
