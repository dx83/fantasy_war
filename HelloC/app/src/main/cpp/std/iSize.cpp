#include "iSize.h"

#include "iStd.h"

iSize iSize::operator+(const iSize& p)
{
	iSize r;
	r.width = width + p.width;
	r.height = height + p.height;
	return r;
}

iSize iSize::operator-(const iSize& p)
{
	iSize r;
	r.width = width - p.width;
	r.height = height - p.height;
	return r;
}

iSize iSize::operator*(const iSize& p)
{
	iSize r;
	r.width = width * p.width;
	r.height = height * p.height;
	return r;
}

iSize iSize::operator/(const iSize& p)
{
	iSize r;
	r.width = width / p.width;
	r.height = height / p.height;
	return r;
}

iSize& iSize::operator+=(const iSize& p)
{
	width += p.width;
	height += p.height;
	return *this;
}

iSize& iSize::operator-=(const iSize& p)
{
	width -= p.width;
	height -= p.height;
	return *this;
}

iSize& iSize::operator*=(const iSize& p)
{
	width *= p.width;
	height *= p.height;
	return *this;
}

iSize& iSize::operator/=(const iSize& p)
{
	width /= p.width;
	height /= p.height;
	return *this;
}

iSize iSize::operator+(const float s)
{
	iSize r;
	r.width = width + s;
	r.height = height + s;
	return r;
}

iSize iSize::operator-(const float s)
{
	iSize r;
	r.width = width - s;
	r.height = height - s;
	return r;
}

iSize iSize::operator*(const float s)
{
	iSize r;
	r.width = width * s;
	r.height = height * s;
	return r;
}

iSize iSize::operator/(const float s)
{
	iSize r;
	r.width = width / s;
	r.height = height / s;
	return r;
}

iSize& iSize::operator+=(const float s)
{
	width += s;
	height += s;
	return *this;
}

iSize& iSize::operator-=(const float s)
{
	width -= s;
	height -= s;
	return *this;
}

iSize& iSize::operator*=(const float s)
{
	width *= s;
	height *= s;
	return *this;
}

iSize& iSize::operator/=(const float s)
{
	width /= s;
	height /= s;
	return *this;
}

iSize iSizeMake(float width, float height)
{
	iSize p;
	p.width = width;
	p.height = height;
	return p;
}

