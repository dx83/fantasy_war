#pragma once

#include "iType.h"
#include "iControl.h"
#include "iRect.h"

//#define PointText

struct Hexagon
{
	iPoint point;
	iRect rt;
	int xIdx, yIdx;			// PointText
	int way[6];				// N, EN, ES, S, WS, WN 시계방향
	int terrain, move, landmark, defBonus = 0;	// TileType->index
	bool exHex;
	int regimentIdx = -1;
};

struct mapOffSet
{
	float mX, mY;
	iPoint curr, max, min;
};

struct LoadMap
{
	// hex
	int totalW, totalH;
	Hexagon* hexs;
	int hMax;

	// map move
	mapOffSet* off;

	Texture** tex;
	iPoint prev;
	bool bMove;

	// map border
	iPoint sp, ep;
	float heightOff, widthOff, cornerW, cornerH;

	// fog
	Texture* texFog;
	bool fogOn, fogHex;

#ifdef PointText
	Texture** pText;
	int tx, ty;
	iSize size;
#endif
};

extern int currentIndex;

void createHexMap();
void freeHexMap();
void drawHexMap(float dt);
bool keyHexMap(iKeyStat stat, iPoint point);
bool allPopMoving();
iPoint& restrictMap(iPoint& off, iPoint min, iPoint max, float mx = 0, float my = 0);

void fogOnOff();
bool fogStatus(int index = -2);

mapOffSet* getMapOffSet();
void setMapOffCurr(iPoint point);
Hexagon* mapKeyInHex();
//Hexagon* getHexInfo(int& max);
Hexagon* getHexInfo();

int getRegimentIndex(int hexIdx);
void setRegimentIndex(int hexIdx, int idx);

