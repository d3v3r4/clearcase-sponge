// DataStore.h

#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <Settings.h>
#include <clearcase/DataEntry.h>
#include <io/OutputStream.h>
#include <thread/Mutex.h>
#include <util/Locker.h>
#include <util/Date.h>

#include <map>
#include <vector>
using namespace std;

class DataStore
{
public:
	DataStore(Settings* settings);
	~DataStore();

	void addData(Date date, FileDiff& fileDiff);
	void addData(Date date, DataEntry& dataEntry);
	void writeToStream(OutputStream& outputStream);

private:
	static String getRow(Date date, DataEntry& dataEntry);
	static String getEmptyRow(Date date);
	Date roundDownDate(Date date);
	Date incrementDate(Date date);
	vector<Date> getMissingDates(Date start, Date end);
	static String getIsoDate(Date date);

	Settings* m_settings;
	Mutex m_mutex;
	map<Date, DataEntry> m_dataMap;
};

#endif // DATA_STORE_H
