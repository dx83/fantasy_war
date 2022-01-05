#pragma once

#include "iStd.h"
#include "LoadMap.h"

struct GameMenu
{
	// bg - map
	Texture* menuBg;
	mapOffSet* off;
	bool bMapMove;
	iPoint dstP, savP;

	float shadowDt;
};

void loadMenu();
void freeMenu();
void drawMenu(float dt);
void keyMenu(iKeyStat stat, iPoint point);

// ==========================================
// popMenu
// ==========================================
void createPopMenu();
void freePopMenu();
void drawPopMenu(float dt);
void showPopMenu(bool show);
bool keyPopMenu(iKeyStat stat, iPoint point);

