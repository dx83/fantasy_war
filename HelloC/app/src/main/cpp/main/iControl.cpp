#include "iControl.h"

#include "iStd.h"

bool* keyBuf;
int keyStat = 0, keyDown = 0;

void keyBufInit()
{
	keyBuf = new bool[256];
	memset(keyBuf, 0x00, 256);	// 키버퍼 0으로 초기화
}

void zeroKeyDown()	// iWindow.cpp => drawApp()
{	// 화면에 뿌릴때마다 키값 초기화
	keyDown = 0;
}

int getKeyStat()
{
	return keyStat;
}

int getKeyDown()
{
	return keyDown;
}

// 연속 입력 지원
void setKeyStat(iKeyStat stat, int c)
{
	setKeys(keyStat, stat, c);
}
// 한 키씩만 입력 받음
void setKeyDown(iKeyStat stat, int c)
{
	if (stat == iKeyStatBegan)
	{
		if (keyBuf[c])	// 같은 입력이면
			return;		// 즉시 함수 종료

		keyBuf[c] = true;
		setKeys(keyDown, stat, c);
	}
	else
	{
		keyBuf[c] = false;
	}
}

void setKeys(int& keys, iKeyStat stat, int c)
{
	if (stat == iKeyStatBegan)
	{
		if (c == 'a' || c == 'A')
			keys |= keysA;
		else if (c == 'd' || c == 'D')
			keys |= keysD;
		else if (c == 'w' || c == 'W')
			keys |= keysW;
		else if (c == 's' || c == 'S')
			keys |= keysS;

		else if (c == ' ')
			keys |= keysSpace;

		if (c == VK_LEFT)
			keys |= keysLeft;
		else if (c == VK_RIGHT)
			keys |= keysRight;
		else if (c == VK_UP)
			keys |= keysUp;
		else if (c == VK_DOWN)
			keys |= keysDown;

		else if (c == VK_ESCAPE)
			keys |= keysESC;
		else if (c == VK_F1)
			keys |= keysF1;
		else if (c == VK_F2)
			keys |= keysF2;
	}
	else// if (stat == iKeyStatEnded)
	{
		if (c == 'a' || c == 'A')
			keys &= ~keysA;		// 1111 1110
		else if (c == 'd' || c == 'D')
			keys &= ~keysD;		// 1111 1101
		else if (c == 'w' || c == 'W')
			keys &= ~keysW;		// 1111 1011
		else if (c == 's' || c == 'S')
			keys &= ~keysS;		// 1111 0111

		else if (c == ' ')
			keys &= ~keysSpace;	// 1110 1111

		if (c == VK_LEFT)
			keys &= ~keysLeft;
		else if (c == VK_RIGHT)
			keys &= ~keysRight;
		else if (c == VK_UP)
			keys &= ~keysUp;
		else if (c == VK_DOWN)
			keys &= ~keysDown;

		else if (c == VK_ESCAPE)
			keys &= ~keysESC;
		else if (c == VK_F1)
			keys &= ~keysF1;
		else if (c == VK_F2)
			keys &= ~keysF2;
	}
}
#if (OS==OS_ANDROID)
bool GetAsyncKeyState(char keyValue)
{
	return false;
}
#endif

