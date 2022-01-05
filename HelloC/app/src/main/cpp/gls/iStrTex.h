#pragma once

#include "iType.h"
#include "iPoint.h"

typedef Texture* (*MethodStrTex)(const char* str);

class iStrTex
{
public:
	iStrTex(MethodStrTex method);
	virtual ~iStrTex();

	static char** getStringLineWidth(const char* str, int& lineNum, float width, int d = '\n');

	void setString(const char* szFormat, ...);

	void paint(float x, float y, int anc, const char* szFormat, ...);
	void paint(iPoint p, int anc, const char* szFormat, ...);

	void paint(float x, float y, int anc);
	void paint(iPoint p, int anc);

public:
	char* str;

	MethodStrTex method;	// 문자열 꾸미는 함수
	Texture* tex;

	float widthMax;
};

