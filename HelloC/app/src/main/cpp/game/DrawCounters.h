#pragma once

#include "iType.h"
#include "iPoint.h"
#include "TextGroup.h"
#include "RegimentMgr.h"

struct CounterTexture
{
	bool ardr;

	Texture** m;		// map counter
	Texture** w;		// war counter
	Texture*** sp;		// small pattern
	Texture*** bp;		// big pattern
	Texture** sh;		// small heraldry
	Texture** bh;		// big heraldry
	
	Texture** tMove;	// mapcounter move
	Texture** tMnum;	// mapcounter ar/dr
	Texture** tWnum;	// warcounter
	
	Texture* selectHex;
	Texture** title;	// army title - general name
	Texture** mark;		// hero mark, x mark
};

extern CounterTexture* counterTex;

struct DrawCounters
{
    int focusIdx;
	int focusRgIdx, currRgIdx;	// focus Regiment / current Regiment or Enemy Regiment
	bool bClickOBJ;				// second cursor

	// ProcUI_Commander.cpp : title
	Texture* heroName;
	bool ally;

	// RegimentAction : battle
	RegimentInfo* rg[2];

	// battleUI center value
	float hcenter;
	float battleW;
};

extern DrawCounters* _dc;

void createCounters();
void freeCounters();
void drawCounters(float dt);

void drawMapCounters(float dt);
void drawSelectHex(float dt);

void drawInfoCounters(float dt, bool subUI);
void drawStepCounters(float dt);
void drawWarCounters(float dt);


