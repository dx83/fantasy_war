#pragma once

#include "iType.h"
#include "iPage.h"
#include "iPopup.h"

struct Intro
{
    iPage* ppt;
	float pressDt;

	Texture** contents;
	float rw, rh;
	
	bool onTouch, onClick;
};

void loadIntro();
void freeIntro();
void drawIntro(float dt);
void keyIntro(iKeyStat stat, iPoint point);

