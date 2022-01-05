#pragma once

#include "iType.h"
#include "iControl.h"
#include "iPopup.h"

struct RegimentInfoTex
{
	Texture** entry;
	Texture** status;
	Texture** steps;
	Texture** roundBox;
};

struct PopRegimentInfo
{
	iPopup** pop;
	Texture** steps;

	bool bTouch, bStep;
};

void createPopRegimentInfo();
void freePopRegimentInfo();
void drawPopRegimentInfo(float dt);
void showPopRegimentInfo(bool show);

void drawStepReference(float dt);
void showStepReference(bool show);

bool keyPopRegimentInfo(iKeyStat stat, iPoint point);

