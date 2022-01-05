#pragma once

#include "iType.h"

enum iKeyStat
{
	iKeyStatBegan = 0,
	iKeyStatMoved,
	iKeyStatEnded
};

#define keysA		 1// 0000 0001 left
#define keysD		 2// 0000 0010 right
#define keysW		 4// 0000 0100 up
#define keysS		 8// 0000 1000 down

#define keysSpace	16// 0001 0000 space

#define keysLeft	32
#define keysRight	64
#define keysUp		128
#define keysDown	256

#define keysESC		512// 0010 0000 esc
#define keysF1		1024
#define keysF2		2048

#if (OS==OS_ANDROID)
#define VK_LEFT     0x25
#define VK_RIGHT    0x27
#define VK_UP       0x26
#define VK_DOWN     0x28
#define VK_ESCAPE   0x1B
#define VK_F1       0x70
#define VK_F2       0x71
#define VK_F5		0x74

#define VK_RBUTTON	0x02

bool GetAsyncKeyState(char keyValue);
#endif

void keyBufInit();
void zeroKeyDown();	// iWindow.cpp => drawApp, 키버퍼 그릴때마다 0으로 초기화
int getKeyStat();
int getKeyDown();
void setKeyStat(iKeyStat stat, int c);			// 연속 입력 지원
void setKeyDown(iKeyStat stat, int c);			// 한 키씩만 입력 받음
void setKeys(int& keys, iKeyStat stat, int c);	// 키 판단

