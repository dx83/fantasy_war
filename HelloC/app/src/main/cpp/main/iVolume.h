#pragma once

#include "iType.h"
#include "iControl.h"
#include "iPopup.h"

struct iVolume
{
	iPopup* volumes;
	Texture* wheel;
	iImage* xButton;
	float pressDt;

	iRect wBtn, xBtn, mBtn, eBtn;
	bool bWheel, bMusic, bEffect;
};

void createPopVolume();
void freePopVolume();
void drawPopVolume(float dt);
void showPopVolume(bool show);
void volumePopup(float dt);
bool keyPopVolume(iKeyStat stat, iPoint point);

