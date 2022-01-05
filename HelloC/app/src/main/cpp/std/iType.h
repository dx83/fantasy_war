#pragma once

#define OS_WINDOWS 0
#define OS_ANDROID 1

#define OS OS_ANDROID

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#if (OS==OS_WINDOWS)

#include <time.h>
#include "iWindow.h"

#elif (OS==OS_ANDROID)

#include <jni.h>
#include <android/log.h>

#include <sys/time.h>

#define GL_ES

#endif

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;

typedef void (*VOID_METHOD)();
typedef void (*FLOAT_METHOD)(float);

struct Texture
{
	uint32 texID;// uint32 == GLuint
	float width, height;
	float potWidth, potHeight;
	int retainCount;
};

struct iPoint;
struct iSize;
struct iRect;

// anc
#define LEFT		 1// 0000 0001
#define RIGHT		 2// 0000 0010
#define HCENTER		 4// 0000 0100

#define TOP			 8// 0000 1000
#define BOTTOM		16// 0001 0000
#define VCENTER		32// 0010 0000

#define REVERSE_NONE	0
#define REVERSE_WIDTH	1
#define REVERSE_HEIGHT	2
