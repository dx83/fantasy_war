#pragma once

#include "iType.h"
#include "iControl.h"
#include "iPopup.h"

extern struct Hexagon;

struct MapKey
{
	// MapKey
	int maxTr, maxLd;

	// Text
	Texture** tLand;
	Texture** tMove;
	Texture** tDef;
	Texture** goal;
	int max;

	// popup
	float w, h;
	iPoint offPop, offPrev;

	bool bMove;
};

extern MapKey* mk;

void createMapKey();
void freeMapKey();
void drawTileInfo(float dt);
void showTileInfo(bool show);
bool keyTileInfo(iKeyStat stat, iPoint point);

bool movingMapkey();

