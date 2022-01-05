#pragma once

#include "iType.h"
#include "iControl.h"
#include "iPopup.h"
extern struct RegimentInfo;

struct TRACKHEX
{
    int index;
    int value;
    int setTex;
};

struct RegimentAction
{
    iPopup** pop;
    Texture** tex;
    TRACKHEX* tHex;

    Texture** uiText;
    Texture** bigNum;
    Texture** status;
    Texture** dice;
    int resDice[4];

    bool bStart, bRetreat;  // 전투개시 / 군단후퇴 버튼
    bool bFirst, bRolling;  //, bFlow;  // 0라운드 / 전투개시->주사위굴림

    int battleRes, battleHit, magicHit;
    bool magicA, magicD;

    float ratioA, ratioD;
    int numA[3], numD[3];
    int resultA, resultD;

    bool THEEND;

    Texture** descText;
    int resultTextIdx, descTextIdx;
    bool bDescL, bDescR;
};

extern bool warscene;

void createRegimentAction();
void freeRegimentAction();
void drawRegimentAction(float dt);
void showRegimentAction(bool show);
bool keyRegimentAction(iKeyStat stat, iPoint point);

void actionRangeTracking();
void drawBattleAction(float dt);
void showBattleAction(bool show);
bool keyBattleAction(iKeyStat stat, iPoint point);

bool battleUIAction();
bool actRegimentAction();

