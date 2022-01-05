#pragma once

#include "iType.h"
#include "iColor.h"

#if (OS==OS_WINDOWS)
#include "iWindow.h"

#if 1// AppForCtrl.cpp
#include <sys/types.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>//WinSock.h �ʹ� ������
#pragma comment(lib, "ws2_32.lib")

#endif

#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
//#pragma comment(lib, "gdiplus")

using namespace Gdiplus;

typedef Image igImage;

#elif (OS==OS_ANDROID)

typedef void igImage;

#include "iNDK.h"

#endif

#include "iSize.h"
#include "iRect.h"


class iGraphics
{
private:
	iGraphics();
public:
	static iGraphics* share();
public:
	virtual ~iGraphics();

#if (OS==OS_WINDOWS)
	// gdiplus
	void start(HDC hdc);
	void end();
#endif

	void init(iSize size);
	void init(int width, int height);

	void clear(float r, float g, float b, float a);
	void clear();

	Texture* getTexture();

	void drawLine(iPoint sp, iPoint ep);
	void drawLine(float x0, float y0, float x1, float y1);

	void drawRect(iRect rt, float radius = 0.0f);
	void drawRect(float x, float y, float width, float height, float radius = 0.0f);
	
	void fillRect(iRect rt, float radius = 0.0f);
	void fillRect(float x, float y, float width, float height, float radius = 0.0f);

	igImage* createIgImage(const char* szFormat, ...);
	void freeIgImage(igImage* img);
	static int igImageGetWidth(igImage* img);
	static int igImageGetHeight(igImage* img);
	void drawIgImage(igImage* img, float x, float y, int anc,
		float ix, float iy, float iw, float ih,
		float rx, float ry,
		int xyz, float degree);
	void drawIgImage(igImage* img, float x, float y, int anc);

	static iRect rectOfString(const char* szFormat, ...);
	void drawString(float x, float y, int anc, const char* szFormat, ...);

#if (OS==OS_WINDOWS)
public:
	Bitmap* bmp;
	Graphics* g;
#endif
};

#if (OS==OS_WINDOWS)
wchar_t* utf8_to_utf16(const char* szFormat, ...);
char* utf16_to_utf8(const wchar_t* str);
#endif

