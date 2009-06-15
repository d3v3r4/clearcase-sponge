// String.cpp

#include "String.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string.h>

// Alias the Windows function to compare ignoring case
#ifdef WINDOWS
#undef stricmp
#define stricmp(x, y) _stricmp((x), (y))
#endif

// If stricmp doesn't exist, use strcasecmp
#ifndef stricmp
#define stricmp(x, y) strcasecmp((x), (y))
#endif


String::String()
{

}

String::String(const String& str)
{
	m_str.assign(str.m_str);
}

String::String(const std::string& str)
{
	m_str.assign(str);
}

String::String(int8 c)
{
	m_str.append(1, c);
}

String::String(int16 n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(int32 n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(int64 n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(uint8 n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(uint16 n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(uint32 n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(uint64 n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(float n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(double n)
{
	stringstream ss;
	ss << n;
	ss >> m_str;
}

String::String(const char* str)
{
	m_str.assign(string(str));
}

String::~String()
{
	
}

void String::append(int8 c)
{
	m_str.append(1, c);
}

void String::append(int16 value)
{
	append(String(value));
}

void String::append(int32 value)
{
	append(String(value));
}

void String::append(int64 value)
{
	append(String(value));
}

void String::append(uint8 value)
{
	append(String(value));
}

void String::append(uint16 value)
{
	append(String(value));
}

void String::append(uint32 value)
{
	append(String(value));
}

void String::append(uint64 value)
{
	append(String(value));
}

void String::append(const char* str)
{
	m_str.append(str);
}

void String::append(const String& str)
{
	m_str.append(str.m_str);
}

void String::assign(const String& str)
{
	m_str.assign(str.m_str);
}

const char* String::c_str() const
{
	return m_str.c_str();
}

char String::charAt(uint32 index) const
{
	return m_str.at(index);
}

void String::clear()
{
	m_str.clear();
}

int32 String::compareTo(const String& str) const
{
	return m_str.compare(str.m_str);
}

int32 String::compareToIgnoringCase(const String& str) const
{
	return stricmp(m_str.c_str(), str.m_str.c_str());
}

bool String::endsWith(const String& suffix) const
{
	return m_str.rfind(suffix.m_str) == m_str.length() - suffix.length();
}

bool String::equals(const String& str) const
{
	return m_str == str.m_str;
}

bool String::equalsIgnoringCase(const String& str) const
{
	if (m_str.length() != str.m_str.length())
		return false;

	return (stricmp(m_str.c_str(), str.m_str.c_str()) == 0);
}

uint32 String::indexOf(char c) const
{
	return (uint32)m_str.find(c);
}

uint32 String::indexOf(const String& str) const
{
	return (uint32)m_str.find(str.m_str);
}

uint32 String::indexOf(char c, uint32 fromIndex) const
{
	return (uint32)m_str.find(c, fromIndex);
}

uint32 String::indexOf(const String& str, uint32 fromIndex) const
{
	return (uint32)m_str.find(str.m_str, fromIndex);
}

void String::insert(uint32 index, const String& str)
{
	m_str.insert(index, str.m_str);
}

uint32 String::lastIndexOf(char c) const
{
	return (uint32)m_str.rfind(c);
}

uint32 String::lastIndexOf(const String& str) const
{
	return (uint32)m_str.rfind(str.m_str);
}

uint32 String::lastIndexOf(char c, uint32 fromIndex) const
{
	return (uint32)m_str.find_last_of(c, fromIndex);
}

uint32 String::lastIndexOf(const String& str, uint32 fromIndex) const
{
	return (uint32)m_str.rfind(str.m_str, fromIndex);
}

uint32 String::length() const
{
	return (uint32)m_str.length();
}

void String::remove(uint32 index, uint32 count)
{
	m_str.erase(index, count);
}

String String::subString(uint32 index) const
{
	return String(m_str.substr(index, m_str.length()-index));
}

String String::subString(uint32 start, uint32 end) const
{
	return String(m_str.substr(start, end-start));
}

bool String::startsWith(const String& prefix) const
{
	return m_str.find(prefix.m_str) == 0;
}

void String::replaceAll(char subThis, char withThis)
{
	std::replace(m_str.begin(), m_str.end(), subThis, withThis);
}

void String::replaceAll(const String& subThis, const String& withThis)
{
	if (subThis.m_str.empty())
		return;

	uint32 lastLoc = 0;
	while (true)
	{
		int32 loc = (int32)m_str.find(subThis.m_str, lastLoc);
		if (loc >= 0)
		{
			m_str.replace(loc, subThis.m_str.length(), withThis.m_str);
			lastLoc = loc + (uint32)withThis.m_str.length();
		}
		else
		{
			return;
		}
	}
}

void String::trim()
{
	if (m_str.length() == 0)
		return;

	size_t start, end;

	// Find start and end of white space
	for (start = 0; start < m_str.length() && isspace(m_str[start]); start++);
	for (end = m_str.length()-1; end >= 0 && isspace(m_str[end]); end--);

	// Skip the rest if there is nothing to do
	if (start == 0 && end == m_str.length()-1)
		return;
 
	m_str.assign(m_str.substr(start, end-start+1));
}

// Numeric functions --------------------------------------------------------

int8 String::toInt8(bool& success) const
{
	stringstream ss;
	int8 ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

int16 String::toInt16(bool& success) const
{
	stringstream ss;
	int16 ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

int32 String::toInt32(bool& success) const
{
	stringstream ss;
	int32 ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

int64 String::toInt64(bool& success) const
{
	stringstream ss;
	int64 ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

uint8 String::toUInt8(bool& success) const
{
	stringstream ss;
	uint8 ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

uint16 String::toUInt16(bool& success) const
{
	stringstream ss;
	uint16 ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

uint32 String::toUInt32(bool& success) const
{
	stringstream ss;
	uint32 ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

uint64 String::toUInt64(bool& success) const
{
	stringstream ss;
	uint64 ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

float String::toFloat(bool& success) const
{
	stringstream ss;
	float ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

double String::toDouble(bool& success) const
{
	stringstream ss;
	double ret;

	ss << m_str;
	ss >> ret;
	success = !ss.bad();
	return ret;
}

// Overloaded operators -----------------------------------------------------

String& String::operator=(const String& other)
{
	m_str.assign(other.m_str);
	return *this;
}


String& String::operator+=(const String& other)
{
	this->append(other);
	return *this;
}


String& String::operator+=(int8 other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(int16 other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(int32 other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(int64 other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(uint8 other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(uint16 other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(uint32 other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(uint64 other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(float other)
{
	this->append(String(other));
	return *this;
}


String& String::operator+=(double other)
{
	this->append(String(other));
	return *this;
}


const String String::operator+(const String& other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(int8 other) const
{
	String ret(*this);
	ret.append(other);
	return ret;
}


const String String::operator+(int16 other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(int32 other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(int64 other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(uint8 other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(uint16 other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(uint32 other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(uint64 other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(float other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


const String String::operator+(double other) const
{
	String ret(*this);
	ret.append(String(other));
	return ret;
}


bool String::operator==(const String& other) const
{
	if (this == &other)
		return true;

	return equals(other);
}


bool String::operator!=(const String& other) const
{
	if (this != &other)
		return true;

	return !equals(other);
}


bool String::operator<(const String& other) const
{
	if (this == &other)
		return false;

	return compareTo(other) < 0;
}


bool String::operator<=(const String& other) const
{
	if (this == &other)
		return true;

	return compareTo(other) <= 0;
}


bool String::operator>(const String& other) const
{
	if (this == &other)
		return false;

	return compareTo(other) > 0;
}


bool String::operator>=(const String& other) const
{
	if (this == &other)
		return true;

	return compareTo(other) >= 0;
}


const char String::operator[](uint32 index) const
{
	return m_str[index];
}

char& String::operator[](uint32 index)
{
	return m_str[index];
}

// Stream functions ---------------------------------------------------------

/*
 * Outputs to the ostream.
 */
ostream& operator<<(ostream& os, String& str)
{
	os << str.m_str;
	return os;
}

/*
 * Reads into the istream.
 */
istream& operator>>(istream& is, String& str)
{
	is >> str.m_str;
	return is;
}
