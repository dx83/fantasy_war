#pragma once

struct iSize
{
	float width, height;

	iSize operator +(const iSize& p);
	iSize operator -(const iSize& p);
	iSize operator *(const iSize& p);
	iSize operator /(const iSize& p);

	iSize& operator +=(const iSize& p);
	iSize& operator -=(const iSize& p);
	iSize& operator *=(const iSize& p);
	iSize& operator /=(const iSize& p);

	iSize operator +(const float s);
	iSize operator -(const float s);
	iSize operator *(const float s);
	iSize operator /(const float s);

	iSize& operator +=(const float s);
	iSize& operator -=(const float s);
	iSize& operator *=(const float s);
	iSize& operator /=(const float s);
};

#define iSizeZero iSizeMake(0, 0)
#define iSizeOne iSizeMake(1, 1)
iSize iSizeMake(float width, float height);
