#include "Proc.h"

#include "Loading.h"
#include "Menu.h"

#include "LoadMap.h"
#include "ProcUI_Commander.h"
#include "ProcUI_MapKey.h"
#include "ProcUI_MiniMap.h"
#include "ProcUI_RegimentInfo.h"

#include "RegimentAction.h"
#include "RegimentMgr.h"
#include "DrawCounters.h"

static PROC* proc;
//bool toMenu;//audioPlay(0);
#if CHECK_TEXTURE
int procTexNum;
#endif
void loadProc()
{
#if CHECK_TEXTURE
    procTexNum = texture_num;
#endif
    createHexMap();

    createMapKey();
    showTileInfo(true);
    createMiniMap();
    showMiniMap(true);

    initRegimentMgr();
    createCounters();

    createPopRegimentInfo();
    showPopRegimentInfo(false);

    createCommander();
    createRegimentAction();
    showCommander(false);
    showRegimentAction(false);
#if 1
    //printf("Regiments Count : %d\n", regimentsTotalCount());
    //loge("Regiments Count : %d\n", regimentsTotalCount());
    Hexagon* hexs = getHexInfo();
    gotoPosition(hexs[802].point - iPointMake(72, 0));
#endif
    createPopOption();
    showPopOption(true);
    createPopPause();
    showPopPause(false);
    showPopDesc(true);
    createPopVolume();
    showPopVolume(false);

    createPopVic();
    showPopVic(false);

    proc = new PROC;
    Method_Draw draw[DRAWMAX] =
    {
        drawTileInfo,
        drawMiniMap,
        drawCommander,
    };
#ifdef _NONE
    proc->draw = new iArray(NULL);

    for (int i = 0; i < DRAWMAX; i++)
    {
        proc->draw->addObject((void*)draw[i]);
        proc->idx[i] = i;
    }
#else

#endif
    //toMenu = false;
    //loge("loadProc END");
}

void freeProc()
{
    delete proc->draw;
    delete proc;

    freeHexMap();

    freeMapKey();
    freeMiniMap();

    freeRegimentMgr();
    freeCounters();

    freePopRegimentInfo();

    freeCommander();
    freeRegimentAction();

    freePopOption();
    freePopPause();
    freePopVolume();

    freePopVic();
#if CHECK_TEXTURE
    loge("PROC ==== prev(%d) curr(%d)", procTexNum, texture_num);
#endif
}

void drawOrder(int order)
{
    // mapkey 0, minimap 1, commander 2
    for (int i = 0; i < DRAWMAX; i++)
    {
        if(proc->idx[i] == order)
        {
            for (int j = i; j > 0; j--)
                proc->idx[j] = proc->idx[j - 1];
            proc->idx[0] = order;
            break;
        }
    }
}

void drawProc(float dt)
{
    //clearRect(0.5f, 0.5f, 0.5f, 1.0f);
    clearRect(0, 0, 0, 1.0f);
    resetRGBA();

    if ( warscene == false )
    {
        drawHexMap(dt);

        drawCounters(dt);
        drawPopRegimentInfo(dt);

        for (int i = 0; i < DRAWMAX; i++)
        {
            Method_Draw d = (Method_Draw)proc->draw->objectAtIndex(proc->idx[DRAWMAX - 1 - i]);
            d(dt);
        }

        drawStepReference(dt);

        drawPopOption(dt);
        drawPopPause(dt);
        volumePopup(dt);
    }

    drawBattleAction(dt);

    drawPopVic(dt);

    resetRGBA();
}

void keyProc(iKeyStat stat, iPoint point)
{
    if (keyBattleAction(stat, point)    ||
        battleUIAction()                ||
        keyPopVic(stat, point)          ||
        keyPopDesc(stat, point)         ||
        keyPopVolume(stat, point)       ||
        keyPopPause(stat, point)        ||
        keyPopOption(stat, point))
        return;
#if 0
    switch (stat)
    {
    case iKeyStatBegan:
        break;
    case iKeyStatMoved:
        break;
    case iKeyStatEnded:
        break;
    }
#endif

    for (int i = 0; i < DRAWMAX; i++)
    {
        switch(proc->idx[i])
        {
            case 0:
                if (keyTileInfo(stat, point))   return;
                break;
            case 1:
                if (keyMiniMap(stat, point))    return;
                break;
            case 2:
                if (keyCommander(stat, point))  return;
                break;
        }
    }

#if 0
    if (keyHexMap(stat, point))
        return;
#endif
    if (keyPopRegimentInfo(stat, point))
        return;

    keyHexMap(stat, point);

    if (keyRegimentAction(stat, point))
        return;
}


// ===================================
// popOption
// ===================================
PopOption* pOption;

void drawPopWheel(iPopup* pop, float dt, float rate);

void createPopOption()
{
    pOption = new PopOption;

    Texture* tex;
    iPopup* pop = new iPopup();
    iImage* img = new iImage();

    iSize size;
    for (int i = 0; i < 2; i++)
    {
        tex = createImage("assets/menu/btn_circle%d.png", i);
        size = iSizeMake(tex->width, tex->height);
        img->addObject(tex);
        freeImage(tex);
    }
    pop->addObject(img);
    pOption->btn = img;

    pop->style = iPopupNone;
    pop->openPoint =
    pop->closePoint = iPointMake(devSize.width - size.width - 10, 10);
    pop->methodTogether = drawPopWheel;
    pop->selected = 0;

    pOption->rt = iRectMake(pop->openPoint.x - 10, 0, size.width + 20, size.height + 20);

    pOption->wheel = createImage("assets/icon_wheel.png");

    pOption->pop = pop;
}

void freePopOption()
{
    freeImage(pOption->wheel);
    delete pOption;
}

void drawPopWheel(iPopup* pop, float dt, float rate)
{
    if (pOption->pop->selected)
        setRGBA(250, 237, 125, 255);
    else
        setRGBA(242, 150, 97, 255);

    Texture* t = pOption->wheel;
    drawImage(t, pOption->rt.size.width / 2 - 10, pOption->rt.size.height / 2 - 10, VCENTER | HCENTER,
              0, 0, t->width, t->height, 0.8, 0.8, 2, 0);

    resetRGBA();
}

void drawPopOption(float dt)
{
    resetRGBA();
    pOption->btn->setTexObject(pOption->pop->selected);
    pOption->pop->paint(dt);
}

void showPopOption(bool show)
{
    pOption->pop->show(show);
}

bool keyPopOption(iKeyStat stat, iPoint point)
{
    int currIdx = currentIndex;
    switch (stat)
    {
        case iKeyStatBegan:
            if (containPoint(point, pOption->rt))
                pOption->pop->selected = 1;
            break;

        case iKeyStatMoved:
            if (!containPoint(point, pOption->rt))
                pOption->pop->selected = 0;
            break;

        case iKeyStatEnded:
            if (pOption->pop->selected)
            {
                playSfxSound(0);
                //cmBmoveBtnOff();//move 풀기
                //currentIndex = _dc->focusIdx;
                showPopPause(true);
            }
            break;
    }

    if(containPoint(point, pOption->rt))
    {
        currentIndex = currIdx;
        return true;
    }

    return false;
}

