#include "iPopWIndow.h"

#include "Loading.h"
#include "Proc.h"
#include "Menu.h"
#include "iPopup.h"

#include "DrawCounters.h"
#include "RegimentMgr.h"
//#include "RegimentAction.h"
#include "ProcUI_Commander.h"

// ===================================
// popPause
// ===================================
iPopup** popPause;
iImage** imgPauseBtn;

void drawPopPauseBefore(iPopup* pop, float dt, float rate);

void createPopPause()
{
    iImage* img;
    Texture* tex;
    iPopup* pop;
    int i, j;

    const char* str[7] =
    {
        "assets/menu/menu_bg.png",
        "assets/xbutton%d.png",

        "MANUAL",
        "MENU",
        "EXIT",
        "FOG",
        "OPTION",
    };

    imgPauseBtn = new iImage * [6];
    popPause = new iPopup * [7];

    // menu background
    pop = new iPopup();
    img = new iImage();
    tex = createImage(str[0]);
    iSize bg = iSizeMake(tex->width, tex->height);
    img->addObject(tex);
    freeImage(tex);
    pop->addObject(img);

    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointMake(devSize.width / 2 - bg.width / 2, devSize.height / 2 - bg.height / 2);
    pop->methodBefore = drawPopPauseBefore;
    popPause[0] = pop;

    // xButton
    pop = new iPopup();
    img = new iImage();
    for (i = 0; i < 2; i++)
    {
        tex = createImage(str[1], i);
        img->addObject(tex);
        freeImage(tex);
    }
    img->rx = img->ry = 1.3;
    pop->addObject(img);
    imgPauseBtn[0] = img;

    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointMake(devSize.width / 2 + 165, devSize.height / 2 - bg.height / 2 - 10);
    popPause[1] = pop;

    // menu rect buttons
    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(177, 77);

    setStringName("assets/font/Roboto-Regular.ttf");
    setStringSize(33);
    setStringBorder(0);
    igImage* igs[5];
    for (i = 0; i < 2; i++)
        igs[i] = g->createIgImage("assets/menu/btn_dia%d.png", i);

    for (i = 0; i < 3; i++)
    {
        pop = new iPopup();
        img = new iImage();
        for (j = 0; j < 2; j++)
        {
            g->init(size);
            setAlpha(255);
            g->drawIgImage(igs[j], 0, 0, TOP | LEFT);

            j == 0 ? setStringRGBA(255, 187, 0, 250) : setStringRGBA(255, 228, 0, 250);
            g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, str[2 + i]);
            tex = g->getTexture();
            img->addObject(tex);
            freeImage(tex);
        }
        img->position = i > 0 ? iPointMake(0, 100 * i + 130) : iPointMake(0, 100 * i);
        pop->addObject(img);
        imgPauseBtn[i + 1] = img;

        pop->style = iPopupAlpha;
        pop->openPoint =
        pop->closePoint = iPointMake(devSize.width / 2 - size.width / 2, devSize.height / 2 - 220);
        popPause[i + 2] = pop;
    }

    // menu circle buttons
    size = iSizeMake(122, 122);

    setStringName("assets/font/Maplestory_Bold.ttf");
    setStringSize(40);
    setStringBorder(0);
    setStringStyle(1);
    for (i = 0; i < 2; i++)
        igs[i+2] = g->createIgImage("assets/menu/btn_stud%d.png", i);
    igs[4] = g->createIgImage("assets/menu/icon_music.png");

    //==FOG
    pop = new iPopup();
    img = new iImage();
    for (i = 0; i < 2; i++)
    {
        g->init(size);
        g->drawIgImage(igs[i+2], 0, 0, TOP | LEFT);

        //i == 0 ? setStringRGBA(0, 0, 0, 255) : setStringRGBA(255, 0, 0, 255);
        setStringRGBA(0, 0, 0, 255);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, str[5]);
        tex = g->getTexture();
        img->addObject(tex);
        freeImage(tex);
    }
    pop->addObject(img);
    imgPauseBtn[4] = img;

    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointMake(devSize.width / 2 - 150, devSize.height / 2 - 130);
    popPause[5] = pop;

    //==MUSIC
    pop = new iPopup();
    img = new iImage();
    for (i = 0; i < 2; i++)
    {
        g->init(size);
        g->drawIgImage((igs[i+2]), 0, 0, TOP | LEFT);

        i == 0 ? setStringRGBA(200, 200, 200, 250) : setStringRGBA(0, 0, 0, 255);
        g->drawIgImage(igs[4], size.width / 2 - 45, size.height / 2 - 45, TOP | LEFT);

        tex = g->getTexture();
        img->addObject(tex);
        freeImage(tex);
    }
    pop->addObject(img);
    imgPauseBtn[5] = img;

    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointMake(devSize.width / 2 + 150 - size.width, devSize.height / 2 - 130);
    popPause[6] = pop;

    for(i=0; i<5; i++)
        g->freeIgImage(igs[i]);

    createPopYesNo();
    createPopDesc();
}

void freePopPause()
{
    for (int i = 0; i < 7; i++)
        delete popPause[i];
    delete popPause;
    delete imgPauseBtn;

    freePopYesNo();
    freePopDesc();
}

void drawPopPauseBefore(iPopup* pop, float dt, float rate)
{
    setRGBA(0, 0, 0, 255 * 0.7f);
    fillRect(-10, -10, devSize.width + 20, devSize.height + 20);
    resetRGBA();

    for (int i = 0; i < 6; i++)
        imgPauseBtn[i]->setTexObject(i == popPause[0]->selected);
}

void drawPopPause(float dt)
{
    for (int i = 0; i < 7; i++)
        popPause[i]->paint(dt);

    drawPopYesNo(dt);
    drawPopDesc(dt);
}

void showPopPause(bool show)
{
    for (int i = 0; i < 7; i++)
        popPause[i]->show(show);
}

bool keyPopPause(iKeyStat stat, iPoint point)
{
    iPopup* pop = popPause[0];
    if (pop->bShow == false)
        return false;
    if (pop->stat != iPopupProc)
        return true;

    int i, j = -1;
    extern PopOption* pOption;
    switch (stat)
    {
        case iKeyStatBegan:
            for (i = 0; i < 6; i++)
            {
                if (containPoint(point, imgPauseBtn[i]->touchRect(popPause[i + 1]->closePoint)))
                {
                    j = i;
                    break;
                }
            }
            if (pop->selected != j)
            {
                //if (j != -1)
                //    audioPlay(2);
                pop->selected = j;
            }
            break;

        case iKeyStatMoved:
            i = pop->selected;
            if (i==-1) break;

            if (!containPoint(point, imgPauseBtn[i]->touchRect(popPause[i + 1]->closePoint)))
                pop->selected = -1;
            break;

        case iKeyStatEnded:
            if (pop->selected == -1)
                break;

            if (pop->selected == 0)
            {
                playSfxSound(0);
                showPopPause(false);
                pOption->pop->selected = 0;
            }
            else if (pop->selected == 1)
            {
                playSfxSound(0);
                showPopDesc(true);
            }
            else if (pop->selected == 2)
            {
                playSfxSound(0);
                showPopYesNo(true, 1);
            }
            else if (pop->selected == 3)
            {
                playSfxSound(0);
                showPopYesNo(true, 0);
            }
            else if (pop->selected == 4)
            {
                playSfxSound(0);
                fogOnOff();
            }
            else if (pop->selected == 5)
            {
                playSfxSound(0);
                showPopVolume(true);
            }

            pop->selected = -1;
            break;
    }

    return true;
}

// ==========================================
// popYesNo
// ==========================================
iPopup* popYesNo = NULL;
iImage* imgMessage;
iImage** imgExitBtn;

void drawPopYesNoBefore(iPopup* pop, float dt, float rate);

static int popMethod;
void createPopYesNo()
{
    if( popYesNo )
        return;

    iImage* img;
    Texture* tex;
    int i, j;

    const char* str[2] =
    {
        "게임을 종료 하시겠습니까?",
        "메뉴로 다시 돌아가겠습니까?",
    };

    iPopup* pop = new iPopup();
    // box
    img = new iImage();
    tex = createImage("assets/menu/box_display.png");
    img->addObject(tex);
    freeImage(tex);
    pop->addObject(img);
    iSize box = iSizeMake(tex->width, tex->height);

    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(480, 120);

    setStringName("assets/font/Roboto-Regular.ttf");
    setStringSize(40);
    setStringRGBA(0, 0, 0, 255);
    setStringBorder(5);
    setStringBorderRGBA(140, 140, 140, 255);
    // word
    img = new iImage();
    for (i = 0; i < 2; i++)
    {
        g->init(size);
        //setRGBA(200, 200, 200, 200);
        //g->fillRect(0, 0, size.width, size.height, 10);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, str[i]);

        tex = g->getTexture();
        img->addObject(tex);
        freeImage(tex);
    }
    img->position = iPointMake(box.width / 2 - size.width / 2, size.height / 2);
    pop->addObject(img);
    imgMessage = img;
    // btn
    imgExitBtn = new iImage*[2];
    for (i = 0; i < 2; i++)
    {
        img = new iImage();
        iSize texSize;
        for (j = 0; j < 2; j++)
        {
            tex = createImage("assets/menu/btn_vx%d_%d.png", i, j);
            texSize = iSizeMake(tex->width, tex->height);
            img->addObject(tex);
            freeImage(tex);
        }

        float pos = i == 0 ? -200 : 200 - texSize.width;
        img->position = iPointMake(box.width / 2 + pos, box.height - 200);

        pop->addObject(img);
        imgExitBtn[i] = img;
    }

    pop->style = iPopupMove;
    pop->openPoint = iPointMake(devSize.width / 2 - box.width / 2, devSize.height / 2 - box.height / 2);
    pop->closePoint = iPointMake(devSize.width / 2 - box.width / 2, devSize.height / 2 - box.height / 2);
    pop->methodBefore = drawPopYesNoBefore;

    popYesNo = pop;
    popMethod = 0;
}

void freePopYesNo()
{
    if (popYesNo == NULL)
        return;
    delete popYesNo;
    popYesNo = NULL;

    delete imgExitBtn;
}

void drawPopYesNoBefore(iPopup* pop, float dt, float rate)
{
    for (int i = 0; i < 2; i++)
        imgExitBtn[i]->setTexObject(i == popYesNo->selected);
}

void drawPopYesNo(float dt)
{
    resetRGBA();
    popYesNo->paint(dt);
}

void showPopYesNo(bool show, int method)
{
    imgMessage->setTexObject(popMethod = method);
    popYesNo->show(show);
}

bool keyPopYesNo(iKeyStat stat, iPoint point)
{
    if(popYesNo == NULL || popYesNo->bShow == false)
        return false;
    if(popYesNo->stat != iPopupProc)
        return true;

    int i, j = -1;
    switch (stat)
    {
    case iKeyStatBegan:
        for (i = 0; i < 2; i++)
        {
            if(containPoint(point, imgExitBtn[i]->touchRect(popYesNo->closePoint)))
            {
                j = i;
                break;
            }
        }
        if(popYesNo->selected != j)
            popYesNo->selected = j;
        break;

    case iKeyStatMoved:
        i = popYesNo->selected;
        if (i==-1) break;

        if(!containPoint(point, imgExitBtn[i]->touchRect(popYesNo->closePoint)))
            popYesNo->selected = -1;
        break;

    case iKeyStatEnded:
        i = popYesNo->selected;
        if(i == -1)
            break;
        popYesNo->selected = -1;

        if(i == 0)
        {
            playSfxSound(0);
            showPopYesNo(false, popMethod);

            if(popMethod == 0)// Exit
            {
                JNIEnv* env = getEnv();
                jclass cls = env->FindClass("java/lang/System");
                jmethodID mid = env->GetStaticMethodID(cls, "exit", "(I)V");
                env->CallStaticVoidMethod(cls, mid, 0);
            }
            else if (popMethod == 1)// Proc to Menu
            {
                audioStop();
                //extern bool toMenu;
                //toMenu = true;
                setLoading(gs_menu, freeProc, loadMenu);
            }
        }
        else if(i == 1)
        {
            playSfxSound(0);
            showPopYesNo(false, popMethod);
        }

        break;
    }
    return true;
}

// ==========================================
// popDesc
// ==========================================
PopDesc* desc;

#define triBtnX 0
#define triBtnY 70
#define triBtnW 125
#define triBtnH 75
#define MAXPAGE 5
void drawDescUI(iPopup* pop, float dt, float rate);

void createPopDesc()
{
    desc = new PopDesc;
    desc->pop = new iPopup*[3];

    iPopup* pop = new iPopup();
    iImage* img = new iImage();
    Texture* tex;

    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(devSize.width, devSize.height);

    // POP : background
    g->init(size);
    setRGBA(50, 50, 50, 220);
    g->fillRect(-5, -5, devSize.width + 10, devSize.height + 10);
    setRGBA(0, 216, 255, 200);
    g->fillRect(120, 50, 900, 540, 20);
    tex = g->getTexture();
    img->addObject(tex);
    freeImage(tex);
    pop->addObject(img);

    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointZero;
    desc->pop[0] = pop;

    //POP : contents
    pop = new iPopup();
    img = new iImage();

    igImage* igs[MAXPAGE];
    for (int i = 0; i < MAXPAGE; i++)
    {
#if GSTAR
        igs[i] = g->createIgImage("assets/intro/description%d.PNG", i);
#else
        igs[i] = g->createIgImage("assets/intro/description%d.PNG", i==4 ? i + 1 : i);
#endif
    }

    tex = createImage("assets/intro/description0.PNG");
    desc->rw = 900 / tex->width;
    desc->rh = 540 / tex->height;
    freeImage(tex);

    resetRGBA();
    size = iSizeMake(900, 540);
    for (int i = 0; i < MAXPAGE; i++)
    {
        g->init(size);
        g->drawIgImage(igs[i], 0, 0, TOP | LEFT,
                       0, 0, 0, 0, desc->rw, desc->rh, 2, 0);

        setRGBA(150, 150, 150, 255);
        setLineWidth(10);
        g->drawRect(1, 1, 900 - 1, 540 - 1, 20);

        tex = g->getTexture();
        img->addObject(tex);
        freeImage(tex);
    }
    pop->addObject(img);
    desc->contents = img;

    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointMake(120, 50);
    desc->pop[1] = pop;
    setLineWidth(1);

    // POP : button
    pop = new iPopup();
    img = new iImage();

    size = iSizeMake(200, 45);
    setStringName("assets/font/RixYeoljeongdo_Regular.ttf");
    setStringSize(35);
    setStringRGBA(255, 255, 255, 220);
    setStringBorder(0);

    g->init(size);
    setRGBA(5, 5, 5, 255);
    g->fillRect(0, 0, size.width, size.height, 10);
    g->drawString(size.width / 2 - 5, size.height / 2,
                  VCENTER | LEFT, "/  %d", MAXPAGE);
    tex = g->getTexture();
    img->addObject(tex);
    img->position = iPointMake(devSize.width / 2 - size.width / 2, 580);
    freeImage(tex);
    pop->addObject(img);

    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointZero;
    pop->methodTogether = drawDescUI;
    desc->pop[2] = pop;

    // page number
    desc->num = new Texture*[MAXPAGE];
    size = iSizeMake(45, 45);
    for (int i = 0; i < MAXPAGE; i++)
    {
        g->init(size);
        g->drawString(size.width / 2, size.height / 2,
                      VCENTER | HCENTER, "%d", i + 1);
        desc->num[i] = g->getTexture();
    }

    // x button
    desc->xBtn = new iImage();
    for (int i = 0; i < 2; i++)
    {
        tex = createImage("assets/xbutton%d.png", i);
        desc->xBtn->addObject(tex);
        desc->xBtn->position = iPointMake(120 + 900 - tex->width / 2, 50 - tex->height / 2);
        freeImage(tex);
    }
    desc->xButton = false;
    // tri button
    desc->tri = createImage("assets/button_tri.png");
    desc->bRight = desc->bLeft = false;

    desc->curr = 0;

    for (int i = 0; i < MAXPAGE; i++)
        g->freeIgImage(igs[i]);
}

void freePopDesc()
{
    for (int i = 0; i < 3; i++)
        delete desc->pop[i];
    delete desc->pop;

    for (int i = 0; i < MAXPAGE; i++)
        delete desc->num[i];
    delete desc->num;

    delete desc->xBtn;

    delete desc;
}

void drawDescUI(iPopup* pop, float dt, float rate)
{
    if ( desc->pop[0]->bShow == false ) desc->curr = 0;
    desc->contents->setTexObject(desc->curr);

    if( desc->bRight )
    {
        setRGBA(250, 237, 125, getAlpha());
        drawImage(desc->tri, 26, devSize.height / 2 - desc->tri->height / 2, TOP | LEFT,
                  0, 0, desc->tri->width, desc->tri->height, 1, 1, 2, 0, REVERSE_WIDTH);
    }
    else
    {
        setRGBA(102, 0, 0, getAlpha());
        drawImage(desc->tri, 26, devSize.height / 2 - desc->tri->height / 2, TOP | LEFT,
                  0, 0, desc->tri->width, desc->tri->height, 1, 1, 2, 0, REVERSE_WIDTH);
    }

    if( desc->bLeft )
    {
        setRGBA(250, 237, 125, getAlpha());
        drawImage(desc->tri, 1014, devSize.height / 2 - desc->tri->height / 2, TOP | LEFT);
    }
    else
    {
        setRGBA(102, 0, 0, getAlpha());
        drawImage(desc->tri, 1014, devSize.height / 2 - desc->tri->height / 2, TOP | LEFT);
    }

    setRGBA(255, 255, 255, getAlpha());
    desc->xBtn->setTexObject(desc->xButton);
    desc->xBtn->paint(dt, 0, 0, 1, 1);

    resetRGBA();
    Texture* t = desc->num[desc->curr];
    drawImage(t, devSize.width / 2 - t->width - 15, 580, TOP | LEFT);
}

void drawPopDesc(float dt)
{
    for (int i = 0; i < 3; i++)
        desc->pop[i]->paint(dt);
}

void showPopDesc(bool show)
{
    for (int i = 0; i < 3; i++)
        desc->pop[i]->show(show);
}

bool keyPopDesc(iKeyStat stat, iPoint point)
{
    iPopup* pop = desc->pop[0];
    if (pop->bShow == false)
        return false;
    if (pop->stat != iPopupProc)
        return true;

    iRect rt;
    switch (stat)
    {
        case iKeyStatBegan:
        {
            if ( containPoint(point, desc->xBtn->touchRect()) )
                desc->xButton = true;

            rt = iRectMake(25, devSize.height / 2 - desc->tri->height / 2, 100, 112);
            if( containPoint(point, rt) )
                desc->bRight = true;

            rt = iRectMake(1015, devSize.height / 2 - desc->tri->height / 2, 100, 112);
            if( containPoint(point, rt) )
                desc->bLeft = true;

            break;
        }

        case iKeyStatMoved:
        {
            if ( !containPoint(point, desc->xBtn->touchRect()) )
                desc->xButton = false;

            rt = iRectMake(25, devSize.height / 2 - desc->tri->height / 2, 100, 112);
            if( !containPoint(point, rt) )
                desc->bRight = false;

            rt = iRectMake(1015, devSize.height / 2 - desc->tri->height / 2, 100, 112);
            if( !containPoint(point, rt) )
                desc->bLeft = false;

            break;
        }

        case iKeyStatEnded:
        {
            if ( desc->xButton )
            {
                playSfxSound(0);
                showPopDesc(false);
            }

            if ( desc->bRight )
            {
                playSfxSound(0);
                desc->curr--;
                if ( desc->curr < 0)
                    desc->curr = 4;
            }
            if (desc->bLeft )
            {
                playSfxSound(0);
                desc->curr++;
                if ( desc->curr > 4 )
                    desc->curr = 0;
            }

            desc->xButton = false;
            desc->bRight = desc->bLeft = false;
            break;
        }
    }

    return true;
}

// ==========================================
// popVic
// ==========================================
iPopup* popVic;
bool victory;
static bool setVictory;

void createPopVic()
{
    iPopup* pop = new iPopup();
    iImage* img = new iImage();
    Texture* tex;

    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(devSize.width, devSize.height);
    setStringName("assets/font/Maplestory_Bold.ttf");
    setStringRGBA(0, 0, 0, 255);
    setStringBorder(5);
    setStringBorderRGBA(153, 112, 0, 255);

    g->init(size);
    setRGBA(0, 216, 255, 200);
    g->fillRect(0, 0, 700, 500, 20);

    setStringSize(70);
    g->drawString(700 / 2, 65, VCENTER | HCENTER, "샨고스 요새 점령 !!");
    setStringSize(50);
#if GSTAR
    g->drawString(135, 150, VCENTER | LEFT, "카운터에서 뽑기 이벤트");
    g->drawString(135, 220, VCENTER | LEFT, "참가권을 받아가세요.");
#else
    g->drawString(135, 150, VCENTER | LEFT, "UI 중심으로 구현하여,");
    g->drawString(135, 220, VCENTER | LEFT, "전투 로직은 간단합니다.");
#endif
    g->drawString(135, 310, VCENTER | LEFT, "게임을 즐겨주셔서");
    g->drawString(135, 385, VCENTER | LEFT, "감사합니다.");
    g->drawString(135, 460, VCENTER | LEFT, "(특전 : 이동력 무한)");

    setRGBA(150, 150, 150, 255);
    setLineWidth(10);
    g->drawRect(1, 1, 700 - 1, 500 - 1, 20);
    tex = g->getTexture();
    img->addObject(tex);
    freeImage(tex);
    pop->addObject(img);

    pop->style = iPopupAlpha;
    pop->openPoint =
    pop->closePoint = iPointMake(220, 70);
    popVic = pop;

    victory = setVictory = false;
}

void freePopVic()
{
    delete popVic;
}

void drawPopVic(float dt)
{
    popVic->paint(dt);
}

void showPopVic(bool show)
{
    popVic->show(show);
    if ( victory && setVictory == false )// 승리 조건 충족 직후에만 트랙킹 끄기(이동무한 적용)
        cmBmoveBtnOff();
}

void victoryCheck()
{
    if ( victory == false )
    {
        if ( currentIndex == 632 )
        {
            RegimentInfo* rg = regimentAtIndex( getRegimentIndex(currentIndex) );
            if ( rg->ally )
                victory = true;
        }
        if ( _dc->focusIdx == 632 )
        {
            RegimentInfo* rg = regimentAtIndex( getRegimentIndex(_dc->focusIdx) );
            if ( rg->ally )
                victory = true;
        }
    }
}

bool keyPopVic(iKeyStat stat, iPoint point)
{
    if ( victory == false )
        return false;

    if ( popVic->stat == iPopupOpen ||
         popVic->stat == iPopupClose)
        return true;

    switch (stat)
    {
        case iKeyStatBegan:
            break;

        case iKeyStatMoved:
            break;

        case iKeyStatEnded:
            iRect rt = iRectMake(0, 0, devSize.width, devSize.height);
            if ( containPoint(point, rt) )
            {
                if ( setVictory == false )
                {
                    int max = regimentsTotalCount();
                    RegimentInfo* rg;
                    for (int i = 0; i < max; i++)
                    {
                        rg = regimentAtIndex(i);
                        rg->mv = 4;
                    }
                }
                setVictory = true;//승리상태 고정
                victory = false;
                showPopVic(false);
            }
            break;
    }

    return true;
}

bool statVictory()// 승리상태 고정
{
    return setVictory;
}

