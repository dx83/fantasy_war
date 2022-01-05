#pragma once

#include "iNDK.h"
//std
#include "iArray.h"
#include "iFPS.h"
#include "iOpenAL.h"
#include "iPoint.h"
#include "iRect.h"
#include "iSize.h"
#include "iString.h"
#include "iType.h"

//gls
#include "iColor.h"
#include "iFBO.h"
#include "iImage.h"
#include "iMatrix.h"
#include "iOpenGL.h"
#include "iPage.h"
#include "iParticleSystem.h"
#include "iPopup.h"
#include "iStrTex.h"

//main
#include "iControl.h"
#include "iPopWindow.h"
#include "iVolume.h"
#include "Sound.h"

//#define DEVELOP
#if (OS==OS_WINDOWS)// OS==OS_MAC
#define DEV_WIDTH 1600//1280//640//1600
#define DEV_HEIGHT 900//720//480//900
#else //(OS==OS_ANDROID || OS==OS_IPHONE)
#define DEV_WIDTH 1136//1280//1600//1280//640//1600
#define DEV_HEIGHT 640//720//900//720//480//900
#define GSTAR 0
#define XPRINT 0
#endif
#define RATE_MONITOR 0.3f

extern iSize devSize;
extern iRect viewport;
extern float rateWidth, rateHeight;

#define va_start_end(szText, szFormat)	\
va_list args; \
va_start(args, szFormat); \
vsnprintf(szText, sizeof(szText), szFormat, args); \
va_end(args)

void appInitialize();

bool setZoom(iPoint point, float scale, float deltaZoom, float deltaDelay);
float updateZoom(float dt, iPoint& p);

void setLineWidth(float width);
float getLineWidth();

void setClip(float x, float y, float width, float height);

void setDotSize(float size);
void drawDot(float x, float y);

void drawCircle(float x, float y, float radius);
void fillCircle(float x, float y, float radius);

void drawLine(iPoint sp, iPoint ep);
void drawLine(float x0, float y0, float x1, float y1);

//void drawRect(const iRect& rt, float radius = 0.0f);
void drawRect(iRect rt, float radius = 0.0f);
void drawRect(float x, float y, float width, float height, float radius = 0.0f);

//void fillRect(const iRect& rt, float radius = 0.0f);
void fillRect(iRect rt, float radius = 0.0f);
void fillRect(float x, float y, float width, float height, float radius = 0.0f);

void drawTri(float x, float y, float width, float height, int reverse = REVERSE_NONE);
void fillTri(float x, float y, float width, float height, int reverse = REVERSE_NONE);

uint32 nextPot(uint32 x);
void copyBmpToRgba(uint32* src, int stride, int x, int y, int w, int h,
                   uint32* dst, int dw, int dh);
Texture* createImageWithRGBA(uint8* rgba, int width, int height);
Texture* createTexture(int width, int height, bool rgba32f = false);	// FBO texture
#define CHECK_TEXTURE 0
#if CHECK_TEXTURE
extern int texture_num;
#endif

Texture** createImageDevide(int numX, int numY, const char* szFormat, ...);
Texture* createImage(const char* szFormat, ...);
void freeImage(Texture* tex);
void drawImage(Texture* tex, float x, float y, int anc,
               float ix, float iy, float iw, float ih,
               float rx, float ry,
               int xyz, float degree, int reverse = REVERSE_NONE);
void drawImage(Texture* tex, float x, float y, int anc);

const char* getStringName();
void setStringName(const char* str);

void setStringSize(float size);
float getStringSize();

void setStringLineHeight(float height);
float getStringLineHeight();

float getStringBorder();
void setStringBorder(float border);

void setStringStyle(int fontStyle);
int getStringStyle();

struct iFont
{
    float height;
    float interval;
    Texture** texs;	// GPU 풀메모리
};
iFont* loadFont(const char* strOTFTTF, float height, const char* strUse);
void freeFont(iFont* font);
void setFont(iFont* font);

char* loadFile(const char* fileName, int& length);
void saveFile(const char* fileName, char* buf, int length);

