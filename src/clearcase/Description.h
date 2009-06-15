// Description.h

#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <text/String.h>

/*
 * Represents the returned data from a call to "cleartool describe"
 */
class Description
{
public:
	Description();
	~Description();

	/*
	 * Populates with data from a call to "cleartool describe"
	 *
	 * Throws ParsingException if there was an error parsing the result.
	 */
	void populate(String versionName, String desc);

	bool m_isInvalid;
	bool m_isDirectory;
	bool m_isSymbolicLink;
	String m_versionName;
	String m_createDate;
	String m_createTime;
	String m_user;
	String m_comment;
	bool m_mergeTo;
};

#endif // DESCRIPTION_H
