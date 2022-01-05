#pragma once

#include "iType.h"
#include "iControl.h"
#include "iPopup.h"

struct Commander
{
    // popCommander
    iPopup** popCom;
    iImage** imgBtn;
    iSize comRt;
    iPoint btnPos;
    // pop control
    iPoint offPop, offPrev;
    bool bMove, bClick;
    // focus
    bool bFocus, saveAlly;
    Texture* saveName;
    // info move
    bool bInfoBtn, bMoveBtn, bSkillBtn;
};

void createCommander();
void freeCommander();
void drawCommander(float dt);
void showCommander(bool show);
bool focusCommander();
bool keyCommander(iKeyStat stat, iPoint point);
bool movingCommander();

void cmBmoveBtnOff();

