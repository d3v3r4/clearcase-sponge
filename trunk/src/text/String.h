// String.h

#ifndef STRING_H
#define STRING_H

#include <ccsponge.h>

#include <string>
using namespace std;

/*
 * Wrapper around std:string to provide some added functionality and better
 * function names.
 *
 * This class is encoding agnostic. It uses whatever the system's 8 bit
 * encoding is. Of course, that means it's not particularly suitable for
 * Windows which wants wchar_t characters.
 *
 * TODO: Need to remove use of stringstream. Visual Studio locks a mutex
 * in its constructor by calling the constructor to _Plocale(). This
 * creates some rare race issues where stringstream fights cout or
 * another std stream. Seems to only occur on first use.
 *
 * TODO: Change to always be in UTF-8 or UTF-16? Have to link some library
 * on unix.
 */
class String
{
public:
	String();
	String(const String& str);

	/*
	 * Creates a String from the passed std::string.
	 */
	String(const std::string& str);

	/*
	 * Creates a String with a single character. Prameter treated as a
	 * character, not a number.
	 */
	String(int8 c);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(int16 n);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(int32 n);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(int64 n);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(uint8 n);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(uint16 n);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(uint32 n);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(uint64 n);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(float f);

	/*
	 * Creates a string initialized with base 10 text of the passed number.
	 */
	String(double d);

	/*
	 * Creates a String from the passed C string.
	 */
	String(const char* str);

	/*
	 * Destroys the String.
	 */
	~String();

	/*
	 * Appends the passed character to the String. Treated as a character,
	 * not a number.
	 */
	void append(int8 c);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(int16 n);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(int32 n);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(int64 n);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(uint8 n);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(uint16 n);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(uint32 n);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(uint64 n);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(float f);

	/*
	 * Appends the passed number to the String in base 10.
	 */
	void append(double d);

	/*
	 * Appends the passed C string to this String.
	 */
	void append(const char* str);

	/*
	 * Appends the String to this String.
	 */
	void append(const String& str);

	/*
	 * Deletes the String's current text and replaces it with a copy of the
	 * passed String's text.
	 */
	void assign(const String& str);

	/*
	 * Returns a null terminated C string. Do not use after modifying the
	 * String.
	 */
	const char* c_str() const;

	/*
	 * Erases all text in this String.
	 */
	void clear();

	/*
	 * Returns the character at the given index.
	 *
	 * WARNING: Character returned may be part of a UTF-8 character.
	 */
	char charAt(uint32 index) const;

	/*
	 * Does a case sensitive string comparison.
	 */
	int32 compareTo(const String& str) const;

	/*
	 * Does a case insensitive string comparison.
	 */
	int32 compareToIgnoringCase(const String& str) const;

	/*
	 * Checks if the String ends with the passed suffix.
	 */
	bool endsWith(const String& suffix) const;

	/*
	 * Does a case sensitive string comparison for equality.
	 */
	bool equals(const String& str) const;

	/*
	 * Does a case insensitive string comparison for equality.
	 */
	bool equalsIgnoringCase(const String& str) const;

	/*
	 * Returns the first index of the given character in a left to right
	 * search, or -1 if it was not found.
	 */
	uint32 indexOf(char c) const;

	/*
	 * Returns the first index of the given String in a left to right
	 * search, or -1 if it was not found.
	 */
	uint32 indexOf(const String& str) const;

	/*
	 * Returns the first index of the given character after the passed
	 * fromIndex in a left to right search, or -1 if it was not found.
	 */
	uint32 indexOf(char c, uint32 fromIndex) const;

	/*
	 * Returns the first index of the given String after the passed
	 * fromIndex in a left to right search, or -1 if it was not found.
	 */
	uint32 indexOf(const String& str, uint32 fromIndex) const;

	/*
	 * Inserts the passed String at the passed index.
	 */
	void insert(uint32 index, const String& str);

	/*
	 * Returns the first index of the given character in a right to left
	 * search, or -1 if it was not found.
	 */
	uint32 lastIndexOf(char c) const;

	/*
	 * Returns the first index of the given String in a right to left
	 * search, or -1 if it was not found.
	 */
	uint32 lastIndexOf(const String& str) const;

	/*
	 * Returns the first index of the given character before the passed
	 * fromIndex in a right to left search, or -1 if it was not found.
	 */
	uint32 lastIndexOf(char c, uint32 fromIndex) const;

	/*
	 * Returns the first index of the given String before the passed
	 * fromIndex in a right to left search, or -1 if it was not found.
	 */
	uint32 lastIndexOf(const String& str, uint32 fromIndex) const;

	/*
	 * Returns the length of the String in bytes.
	 */
	uint32 length() const;

	/*
	 * Removes a range of bytes from the String.
	 *
	 * WARNING: Can create invalid UTF-8 text.
	 */
	void remove(uint32 index, uint32 count);

	/*
	 * Returns a String that is the substing of this String from index to the
	 * end of the String. Equivalent to subString(index, length()-1).
	 *
	 * WARNING: Can create invalid UTF-8 text.
	 */
	String subString(uint32 index) const;

	/*
	 * Returns a String that is the substing of this String from start to end.
	 * The character at the end index is not included in the result.
	 *
	 * WARNING: Can create invalid UTF-8 text.
	 */
	String subString(uint32 start, uint32 end) const;

	/*
	 * Checks if the String starts with the passed prefix.
	 */
	bool startsWith(const String& prefix) const;

	/*
	 * Replaces all instances of the passed character with a replacement
	 * character.
	 *
	 * WARNING: Can create invalid UTF-8 text if what or with are > 127.
	 */
	void replaceAll(char what, char with);

	/*
	 * Replaces all instances of the passed String with a replacement
	 * String in a left to right search.
	 */
	void replaceAll(const String& what, const String& with);

	/*
	 * Removes white space characters from both ends of the String.
	 */
	void trim();

	/*
	 * Attempts to convert the String to an int8 number. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	int8 toInt8(bool& success) const;

	/*
	 * Attempts to convert the String to an int16 number. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	int16 toInt16(bool& success) const;

	/*
	 * Attempts to convert the String to an int32 number. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	int32 toInt32(bool& success) const;

	/*
	 * Attempts to convert the String to an int64 number. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	int64 toInt64(bool& success) const;

	/*
	 * Attempts to convert the String to an uint8 number. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	uint8 toUInt8(bool& success) const;

	/*
	 * Attempts to convert the String to an uint16 number. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	uint16 toUInt16(bool& success) const;

	/*
	 * Attempts to convert the String to an uint32 number. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	uint32 toUInt32(bool& success) const;

	/*
	 * Attempts to convert the String to an int8 number. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	uint64 toUInt64(bool& success) const;

	/*
	 * Attempts to convert the String to a float. If the conversion succeeds,
	 * success will be set to true and the value returned. If the conversion
	 * fails, sucess will be set to false and 0 will be returned.
	 */
	float toFloat(bool& success) const;

	/*
	 * Attempts to convert the String to a double. If the conversion
	 * succeeds, success will be set to true and the value returned. If the
	 * conversion fails, sucess will be set to false and 0 will be returned.
	 */
	double toDouble(bool& success) const;

	// Operators
	String& operator=(const String& other);
	String& operator+=(const String& other);
	String& operator+=(int8 other); // treated as a character, not a number
	String& operator+=(int16 other);
	String& operator+=(int32 other);
	String& operator+=(int64 other);
	String& operator+=(uint8 other);
	String& operator+=(uint16 other);
	String& operator+=(uint32 other);
	String& operator+=(uint64 other);
	String& operator+=(float other);
	String& operator+=(double other);
	const String operator+(const String& other) const;
	const String operator+(int8 other) const; // treated as a character, not a number
	const String operator+(int16 other) const;
	const String operator+(int32 other) const;
	const String operator+(int64 other) const;
	const String operator+(uint8 other) const;
	const String operator+(uint16 other) const;
	const String operator+(uint32 other) const;
	const String operator+(uint64 other) const;
	const String operator+(float other) const;
	const String operator+(double other) const;
	bool operator==(const String& other) const;
	bool operator!=(const String& other) const;
	bool operator<(const String& other) const;
	bool operator<=(const String& other) const;
	bool operator>(const String& other) const;
	bool operator>=(const String& other) const;
	const char operator[](uint32 index) const;
	char& operator[](uint32 index);

private:
	std::string m_str;

friend ostream& operator<<(ostream& os, String& s);
friend istream& operator>>(istream& is, String& s);
};

// Stream functions
ostream& operator<<(ostream& os, String& s);
istream& operator>>(istream& is, String& s);

#endif // STRING_H
