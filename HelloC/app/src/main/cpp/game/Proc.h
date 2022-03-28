#pragma once

#include "iStd.h"

void loadProc();
void freeProc();
void drawOrder(int order);
void drawProc(float dt);
void keyProc(iKeyStat stat, iPoint point);

typedef void (*Method_Draw)(float dt);

#define DRAWMAX 3
struct PROC
{
#ifdef _NONE
    iArray* draw;
#else
    void* draw;
#endif
    int idx[DRAWMAX];
};


// ===================================
// popOption
// ===================================
struct PopOption
{
    iPopup* pop;
    iImage* btn;
    Texture* wheel;
    iRect rt;
};
void createPopOption();
void freePopOption();
void drawPopOption(float dt);
void showPopOption(bool show);
bool keyPopOption(iKeyStat stat, iPoint point);

