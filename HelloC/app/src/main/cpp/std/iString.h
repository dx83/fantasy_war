#pragma once

class iString
{
public:
	iString();
	iString(const char* szFormat, ...);
	iString(const iString& s);
	virtual ~iString();

	iString& operator =(const char* str);
	iString& operator =(const iString& str);

	const char* operator +(const char* s);
	const char* operator +(const iString& s);

	iString& operator += (const char* s);
	iString& operator += (const iString& s);

	void set(const char* szFormat, ...);
	void add(const char* szFormat, ...);
	static char** getStringLine(const char* str, int& lineNum, int d='\n');
	static void freeStringLine(char** line, int lineNum);

	static bool isUnicode(const char* str);

public:
	char* str;
	int strLength;
};

