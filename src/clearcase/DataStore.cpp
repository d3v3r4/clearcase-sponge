// DataStore.cpp

#include "DataStore.h"

DataStore::DataStore(Settings* settings)
{
	m_settings = settings;
}

DataStore::~DataStore()
{

}

void DataStore::addData(Date date, FileDiff& fileDiff)
{
	Locker locker(m_mutex);

	// Round down the date as appropriate to make a good key
	date = roundDownDate(date);

	// Grab the existing entry if it already exists
	map<Date, DataEntry>::iterator iter = m_dataMap.find(date);

	if (iter == m_dataMap.end())
	{
		// If there is no entry for the given time, add an entry
		DataEntry dataEntry(fileDiff);
		m_dataMap.insert(pair<Date, DataEntry>(date, dataEntry));
	}
	else
	{
		// If an entry exists, then replace it with an entry that has the
		// new data added to it
		pair<Date, DataEntry> dataPair = *iter;
		dataPair.second.add(fileDiff);
		m_dataMap.erase(iter);
		m_dataMap.insert(dataPair);
	}
}

void DataStore::addData(Date date, DataEntry& dataEntry)
{
	Locker locker(m_mutex);

	// Round down the date as appropriate to make a good key
	roundDownDate(date);

	// Grab the existing entry if it already exists
	map<Date, DataEntry>::iterator iter = m_dataMap.find(date);

	if (iter == m_dataMap.end())
	{
		// If there is no entry for the given time, add an entry
		m_dataMap.insert(pair<Date, DataEntry>(date, dataEntry));
	}
	else
	{
		// If an entry exists, then add the new data to it
		pair<Date, DataEntry> pair = *iter;
		DataEntry existingEntry = pair.second;
		existingEntry.add(dataEntry);
	}
}

void DataStore::writeToStream(OutputStream& outputStream)
{
	Locker locker(m_mutex);

	// Print the header line
	String header("Date,Lines Added,Lines Changed,Lines Removed,Total Lines\n");
	outputStream.write(header.c_str(), header.length());

	Date prevDate(0);
	map<Date, DataEntry>::iterator iter = m_dataMap.begin();

	// Print out each DataEntry
	while (iter != m_dataMap.end())
	{
		// Extract the next entry
		pair<Date, DataEntry> pair = *iter;
		Date entryDate = pair.first;
		DataEntry entry = pair.second;

		// Before we write it to the stream, Fill in missing dates between
		// entries with zeroed data. For example, if this entry was 2 months
		// after the last one, fill in the empty months.
		if (prevDate != 0)
		{
			vector<Date> missingDates = getMissingDates(prevDate, entryDate);

			for (uint32 i = 0; i < missingDates.size(); i++)
			{
				Date missingDate = missingDates.at(i);
				String emptyRow = getEmptyRow(missingDate);
				outputStream.write(emptyRow.c_str(), emptyRow.length());
			}
		}

		// Write it to the stream
		String row = getRow(entryDate, entry);
		outputStream.write(row.c_str(), row.length());

		prevDate = entryDate;
		iter++;
	}
}

// Private functions --------------------------------------------------------

String DataStore::getRow(Date date, DataEntry& dataEntry)
{
	String formattedDate = getIsoDate(date);
	uint32 added = dataEntry.getLinesAdded();
	uint32 changed = dataEntry.getLinesChanged();
	uint32 removed = dataEntry.getLinesRemoved();

	String ret;
	ret.append(formattedDate);
	ret.append(",");
	ret.append(added);
	ret.append(",");
	ret.append(changed);
	ret.append(",");
	ret.append(removed);
	ret.append(",");
	ret.append(added + changed + removed);
	ret.append("\n");
	return ret;
}

String DataStore::getEmptyRow(Date date)
{
	String formattedDate = getIsoDate(date);

	String ret;
	ret.append(formattedDate);
	ret.append(",0,0,0,0\n");
	return ret;
}

Date DataStore::roundDownDate(Date date)
{
	Settings::timePeriod period = m_settings->getPeriod();

	switch (period)
	{
		case Settings::MONTHLY:
			date.roundToMonth();
		case Settings::WEEKLY:
			date.roundToWeek();
		case Settings::DAILY:
			date.roundToDay();
		//default:
			//Logging::traceln(String("DataStore::roundDownDate") +
			//	" unexpected period setting " + (int32)period);
	}

	return date;
}

Date DataStore::incrementDate(Date date)
{
	Settings::timePeriod period = m_settings->getPeriod();

	switch (period)
	{
		case Settings::MONTHLY:
			date.addMonths(1);
		case Settings::WEEKLY:
			date.addWeeks(1);
		case Settings::DAILY:
			date.addDays(1);
		//default:
		//	Logging::traceln(String("DataStore::incrementDate") +
		//		" unexpected period setting " + (int32)period);
	}

	return date;
}

vector<Date> DataStore::getMissingDates(Date start, Date end)
{
	vector<Date> ret;

	Date temp = incrementDate(start);

	while (temp < end)
	{
		ret.push_back(temp);
		temp = incrementDate(temp);
	}

	return ret;
}

String DataStore::getIsoDate(Date date)
{
	uint32 year = date.getYear();
	uint32 month = date.getMonth()+1;
	uint32 dayOfMonth = date.getDayOfMonth();

	String ret;
	ret.append(year);
	ret.append('-');
	if (month < 10)
		ret.append('0');
	ret.append(month);
	ret.append('-');
	if (dayOfMonth < 10)
		ret.append('0');
	ret.append(dayOfMonth);
	return ret;
}
