#pragma once

#include "iType.h"

struct iColor4b;

struct iColor4f		// 0 ~ 1 (%), 직관적, 무거움
{
	float r, g, b, a;
	iColor4f operator =(const iColor4b& c);
};

struct iColor4b		// 0 ~ 255, 가벼움, 타 플랫폼 이동에 주로 이용
{
	uint8 r, g, b, a;
	iColor4b operator =(const iColor4f& c);
};

iColor4f iColor4fMake(float r, float g, float b, float a);
iColor4b iColor4bMake(uint8 r, uint8 g, uint8 b, uint8 a);

void initRGBA();
void freeRGBA();

float uint8ToFloat(uint8 c);
uint8 floatToUint8(float c);

void setRGBA(iColor4f c);
void setRGBA(iColor4b c);
void setRGBA(uint8 r, uint8 g, uint8 b, uint8 a);
void getRGBA(float& r, float& g, float& b, float& a);
void getRGBA(uint8& r, uint8& g, uint8& b, uint8& a);

void setStringRGBA(uint8 r, uint8 g, uint8 b, uint8 a);
void getStringRGBA(uint8& r, uint8& g, uint8& b, uint8& a);
void setStringBorderRGBA(uint8 r, uint8 g, uint8 b, uint8 a);
void getStringBorderRGBA(uint8& r, uint8& g, uint8& b, uint8& a);

uint8 getAlpha();
void setAlpha(uint8 a);

void clearRect(float r, float g, float b, float a);
void resetRGBA();

