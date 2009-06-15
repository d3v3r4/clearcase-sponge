// CodePoint.h

#ifndef CODE_POINT_H
#define CODE_POINT_H

class CodePoint
{
public:
	CodePoint(uint32 cp);
	~CodePoint();

	bool isHighSurrogate();
	bool isLowSurrogate();

private:
	uint32 m_point;
};

#endif // CODE_POINT_H
