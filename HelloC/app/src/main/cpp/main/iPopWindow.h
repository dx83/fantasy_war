#pragma once

#include "iStd.h"

// ==========================================
// popPause
// ==========================================
void createPopPause();
void freePopPause();
void drawPopPause(float dt);
void showPopPause(bool show);
bool keyPopPause(iKeyStat stat, iPoint point);

// ==========================================
// popYesNo
// ==========================================
void createPopYesNo();
void freePopYesNo();
void drawPopYesNo(float dt);
void showPopYesNo(bool show, int method = 0);
bool keyPopYesNo(iKeyStat stat, iPoint point);

// ==========================================
// popDesc
// ==========================================
void createPopDesc();
void freePopDesc();
void drawPopDesc(float dt);
void showPopDesc(bool show);
bool keyPopDesc(iKeyStat stat, iPoint point);

struct PopDesc
{
    iPopup** pop;

    iImage* contents;
    float rw, rh;
    int curr;
    Texture** num;

    iImage* xBtn;
    bool xButton;

    Texture* tri;
    bool bRight, bLeft;
};

// ==========================================
// popVic
// ==========================================
void createPopVic();
void freePopVic();
void drawPopVic(float dt);
void showPopVic(bool show);
bool keyPopVic(iKeyStat stat, iPoint point);

void victoryCheck();
bool statVictory();
extern bool victory;

