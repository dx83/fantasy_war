#pragma once

#include "iType.h"

#include "iSize.h"
#include "LoadMap.h"

#define triBtnX 40
#define triBtnY 3
#define triBtnW 20
#define triBtnH 20

struct MiniMap
{
	// miniMap
	Texture* tex;
	float rx, ry;
	iSize dI, sc, bx;	// origin, screen, box

	// triBtn
	bool bHide;

	// map move
	mapOffSet* off;
	iPoint offPop, offPrev;

	bool bMove, bClick;

	// minimap(hide->open) reverse open
	float r, p;	// r 열리는 동안 시간 비율, p 미니맵 초기위치
	bool bEnd;	// 팝업창 열림 종료
	bool miniOn;// 미니맵 완전히 닿힐 때 선택박스 끄기
};

void createMiniMap();
void freeMiniMap();
void drawMiniMap(float dt);
void showMiniMap(bool show);
bool keyMiniMap(iKeyStat stat, iPoint point);

bool movingMinimap();
void gotoPosition(iPoint dst, iPoint offPop = iPointZero);

