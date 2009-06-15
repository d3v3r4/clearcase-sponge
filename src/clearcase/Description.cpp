// Description.cpp

#include "Description.h"
#include <exception/ParsingException.h>

// Example file description
/*
version "/vobs/sw/happy_xml.xml@@/main/xml_update/3"
  created 2006-09-08T16:57:14-07 by zoolander.Domain Users@HOBO-HUB
   "A big update to our happy XML file"
   Element Protection:
     User : UNIX:UID-1113 : r--
     Group: users    : r--
     Other:          : r--
   element type: xml
   predecessor version: /main/xml_update/2
   Hyperlinks:
     Merge -> /vobs/sw/happy_xml.xml@@/main/293
     Merge <- /vobs/sw/happy_xml.xml@@/main/292
*/

// Example symbolic link description
/*
symbolic link ".\AdventNet_Turin\download\custom\ciena\download" -> .
  created 2005-08-23T20:52:54-07:00 by Bob Bobber (bbobber.users@xkcd)
  master replica: sw_CA@\sw
  Protection:
    User : BOB\bbobber : rwx
    Group: BUILTIN\Users : rwx
    Other:          : rwx
*/


Description::Description()
{
	m_isInvalid = true;
	m_isDirectory = false;
	m_isSymbolicLink = false;
	m_mergeTo = false;
}

Description::~Description()
{

}

void Description::populate(String versionName, String desc)
{
	m_isInvalid = true;

	// Skip warning on systems that have configuration issues
	// Can start with a string like:
	// noname: Warning: Can not find a group named "xxx\yyy"
	if (desc.startsWith("noname:"))
	{
		desc = desc.subString(desc.indexOf('\n') + 1);
	}

	// Store the version name
	m_versionName.assign(versionName);

	// First words incicate the file type
	m_isDirectory = desc.startsWith("directory");
	m_isSymbolicLink = desc.startsWith("symbolic link");

	// Skip to the end of the first line as we already have the version name
	int dateStart = desc.indexOf('\n');
	if (dateStart < 1)
	{
		throw ParsingException(String("Failed to find date for : ") + versionName);
	}

	// Find the word "created" to find the start of the date
	dateStart = desc.indexOf("created ", dateStart);
	if (dateStart < 0)
	{
		throw ParsingException(String("Failed to find \"created \" for :") + versionName);
	}

	dateStart += strlen("created ");

	// The character 'T' marks the separator between the date and time
	int dateEnd = desc.indexOf('T', dateStart);
	if (dateEnd < 0)
	{
		throw ParsingException(String("Failed to parse \"created \" date for: ") + versionName);
	}

	// Extract the date
	m_createDate.assign(desc.subString(dateStart, dateEnd));

	// The time is after the date, it ends with either a space or a timezone offset (-7:00)
	int timeEnd = desc.indexOf('-', dateEnd-1);
	if (timeEnd < 0)
	{
		throw ParsingException(String("Failed to parse \"created \" date for: ") + versionName);
	}

	m_createTime.assign(desc.subString(dateEnd+1, timeEnd));

	// The full user name is not always there so we go for the short version
	// There are two varients. If the full name is present the user name is
	// in parentheses.
	int userEnd = desc.indexOf('.', timeEnd);
	int userStart = max<int>(desc.lastIndexOf('(', userEnd), desc.lastIndexOf(' ', userEnd));

	if (userStart < 0 || userEnd < 0)
	{
		throw ParsingException(String("Failed to parse user name for: ") + versionName);
	}

	m_user.assign(desc.subString(userStart+1, userEnd));

	// Start looking for the comment. Skip to the end of the line
	int commentStart = desc.indexOf('\n', dateEnd);
	if (commentStart < 0)
	{
		throw ParsingException(String("Failed to find newline after date for: ") + versionName);
	}

	// Find the starting quote of the comment
	commentStart = desc.indexOf('\"', commentStart);

	//  So now we get to the tricky business of finding the end of the comment
	// The comment can contain quotes and newlines so what we do is just work
	// backwords from the very end of the description for a "element type"
	// which follows the comment and then backwards again for a quote.
	int commentEnd = max<int>(desc.lastIndexOf("element type"), desc.lastIndexOf("Protection"));

	if (commentEnd < 0)
	{
		throw ParsingException(String("Failed to find text \"element type\""
			" or \"Protection\" for file: ") + versionName);
	}

	commentEnd = desc.lastIndexOf('\"', commentEnd);

	// Copy the comment (assuming we found one)
	if (commentStart > 0 &&
		commentEnd > 0 &&
		commentStart != commentEnd)
	{
		m_comment.assign(desc.subString(commentStart+1, commentEnd));
	}

	// Look for the text indicating this was merged to after the comment
	int mergeToStart = desc.indexOf("Merger <-", commentEnd);

	// If we found the index then the version was merged to
	m_mergeTo = (mergeToStart > 0);

	m_isInvalid = false;
}
