#include "iColor.h"

#include "iStd.h"

static iColor4b* _colors;

iColor4f iColor4fMake(float r, float g, float b, float a)
{
	iColor4f c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

iColor4b iColor4bMake(uint8 r, uint8 g, uint8 b, uint8 a)
{
	iColor4b c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

// 4b => 4f
iColor4f iColor4f::operator =(const iColor4b& c)
{
	iColor4f r;
	r.r = c.r / 255.f;
	r.g = c.g / 255.f;
	r.b = c.b / 255.f;
	r.a = c.a / 255.f;
	return r;
}
// 4f => 4b
iColor4b iColor4b::operator =(const iColor4f& c)
{
	iColor4b r;
	r.r = c.r * 0xFF;
	r.g = c.g * 0xFF;
	r.b = c.b * 0xFF;
	r.a = c.a * 0xFF;
	return r;
}

void initRGBA()
{
	_colors = new iColor4b[3];
	for (int i = 0; i < 3; i++)
	{
		_colors[i].r = 255;
		_colors[i].g = 255;
		_colors[i].b = 255;
		_colors[i].a = 255;
	}
}

void freeRGBA()
{
	delete _colors;
}

float uint8ToFloat(uint8 c)
{
	return c / 255.0f;
}

uint8 floatToUint8(float c)
{
	return (uint8)(c * 0xFF);
}

void setRGBA(iColor4f c)
{
	_colors[0].r = floatToUint8(c.r);
	_colors[0].g = floatToUint8(c.g);
	_colors[0].b = floatToUint8(c.b);
	_colors[0].a = floatToUint8(c.a);
}

void setRGBA(iColor4b c)
{
	_colors[0].r = c.r;
	_colors[0].g = c.g;
	_colors[0].b = c.b;
	_colors[0].a = c.a;
}

void setRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
	_colors[0].r = r;
	_colors[0].g = g;
	_colors[0].b = b;
	_colors[0].a = a;
}

void getRGBA(float& r, float& g, float& b, float& a)
{
	r = uint8ToFloat(_colors[0].r);
	g = uint8ToFloat(_colors[0].g);
	b = uint8ToFloat(_colors[0].b);
	a = uint8ToFloat(_colors[0].a);
}

void getRGBA(uint8& r, uint8& g, uint8& b, uint8& a)
{
	r = _colors[0].r;
	g = _colors[0].g;
	b = _colors[0].b;
	a = _colors[0].a;
}

void setStringRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
	_colors[1].r = r;
	_colors[1].g = g;
	_colors[1].b = b;
	_colors[1].a = a;
}

void getStringRGBA(uint8& r, uint8& g, uint8& b, uint8& a)
{
	r = _colors[1].r;
	g = _colors[1].g;
	b = _colors[1].b;
	a = _colors[1].a;
}

void setStringBorderRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
	_colors[2].r = r;
	_colors[2].g = g;
	_colors[2].b = b;
	_colors[2].a = a;
}

void getStringBorderRGBA(uint8& r, uint8& g, uint8& b, uint8& a)
{
	r = _colors[2].r;
	g = _colors[2].g;
	b = _colors[2].b;
	a = _colors[2].a;
}

uint8 getAlpha()
{
	return _colors[0].a;
}

void setAlpha(uint8 a)
{
	for (int i = 0; i < 3; i++)
		_colors[i].a = a;
}

void clearRect(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void resetRGBA()
{
	for (int i = 0; i < 3; i++)
	{
		_colors[i].r = 255;
		_colors[i].g = 255;
		_colors[i].b = 255;
		_colors[i].a = 255;
	}
}

