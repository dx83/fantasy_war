#pragma once

#include "iStd.h"
#include "iControl.h"

#define gs_intro	0
#define gs_menu		1
#define gs_proc		2

#define gs_delay	9999
#define gs_test		99

#define gs_loading	0xff

extern int gameState, toGameState;

typedef void (*MethodLoad)();

void setLoading(int gameState, MethodLoad free, MethodLoad load, int methods = 0);
void drawLoading(float dt);
bool keyLoading(iKeyStat state, iPoint point);
int getLoadStep();

void loadStrAppear();
void freeStrAppear();
void drawStrAppear();


void loadProgress();
void freeProgress();
void drawProgress(float dt);
void setProgress(int tick);

//#define StressTest

