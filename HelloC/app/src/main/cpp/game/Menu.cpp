#include "Menu.h"

#include "Loading.h"
#include "Intro.h"
#include "Proc.h"

static GameMenu* gm;
void autoMapMoving(float dt);

#ifdef StressTest//Loading.h
float menuTestDt;
#endif//StressTest

#if CHECK_TEXTURE
int menuTexNum;
#endif
void loadMenu()
{
#if CHECK_TEXTURE
    menuTexNum = texture_num;
#endif
    gm = new GameMenu;
    gm->menuBg = createImage("assets/map/game_map_full.png");
    gm->off = new mapOffSet;
    gm->off->mX = 48, gm->off->mY = 80;
    gm->off->max = iPointZero;
    gm->off->min = iPointMake(devSize.width, devSize.height) - iPointMake(gm->menuBg->width - 96, gm->menuBg->height - 120);
    gm->off->curr = iPointZero;

    gm->bMapMove = false;
    gm->dstP = iPointZero;

    gm->shadowDt = 0.0f;

    createPopMenu();
    showPopMenu(true);

    createPopVolume();
    showPopVolume(false);
#ifdef StressTest
    menuTestDt = 0.0f;
#endif//StressTest
}

void freeMenu()
{
    freeImage(gm->menuBg);
    delete gm->off;
    delete gm;
    freePopMenu();
    freePopVolume();
#if CHECK_TEXTURE
    loge("MENU ==== prev(%d) curr(%d)", menuTexNum, texture_num);
#endif
}

void drawMenu(float dt)
{
    clearRect(0.5f, 0.5f, 0.5f, 1.0f);
    resetRGBA();

    Texture* t = gm->menuBg;
    drawImage(t, gm->off->curr.x, gm->off->curr.y, TOP | LEFT,
        gm->off->mX, gm->off->mY, t->width, t->height, 1, 1, 2, 0);

    autoMapMoving(dt);
    drawPopMenu(dt);

    drawPopVolume(dt);
    resetRGBA();
#ifdef StressTest//스트레스 테스트, intro.cpp
    if (menuTestDt < 2.0f)
    {
        menuTestDt += dt;
        if (menuTestDt >= 2.0f)
            setLoading(gs_intro, freeMenu, loadIntro);
    }
#endif//StressTest
}

void keyMenu(iKeyStat stat, iPoint point)
{
    if( keyPopVolume(stat, point) ||
        keyPopMenu(stat, point))
        return;

    switch (stat)
    {
    case iKeyStatBegan:
        //printf("menu began(%f,%f)\n", point.x, point.y);
        break;

    case iKeyStatMoved:
        break;

    case iKeyStatEnded:
        break;
    }
}

void autoMapMoving(float dt)
{
    if (gm->bMapMove)
    {
        float speed = 60 * dt;
        if (gm->dstP.x < 0)
        {
            gm->off->curr.x -= speed;
            if (gm->off->curr.x < gm->dstP.x)
                gm->off->curr.x = gm->dstP.x;
        }
        else if (gm->dstP.x > 0)
        {
            gm->off->curr.x += speed;
            if (gm->off->curr.x > gm->dstP.x)
                gm->off->curr.x = gm->dstP.x;
        }
        if (gm->dstP.y < 0)
        {
            gm->off->curr.y -= speed;
            if (gm->off->curr.y < gm->dstP.y)
                gm->off->curr.y = gm->dstP.y;
        }
        else if (gm->dstP.y > 0)
        {
            gm->off->curr.y += speed;
            if (gm->off->curr.y > gm->dstP.y)
                gm->off->curr.y = gm->dstP.y;
        }

        restrictMap(gm->off->curr, gm->off->min, gm->off->max);

        if (gm->off->curr == gm->savP)
            gm->bMapMove = false;
        
        gm->savP = gm->off->curr;
    }
    else
    {
        gm->bMapMove = true;
        int x = -random() % 3984;
        int y = -random() % 2040;
        gm->dstP = iPointMake(x, y);

        if (gm->off->curr.x < gm->dstP.x)
            gm->dstP.x = -gm->dstP.x;
        if (gm->off->curr.y < gm->dstP.y)
            gm->dstP.y = -gm->dstP.y;
    }
}

// ===================================
// popMenu
// ===================================
iPopup** popMenu;
iImage** imgMenuBtn;

void drawPopMenuBefore(iPopup* pop, float dt, float rate);

void createPopMenu()
{
    iImage* img;
    Texture* tex;
    iPopup* pop;
    int i, j;

    const char* str[3] =
    {
        "PLAY",
        "ABOUT",
        "EXIT",
    };

    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(177, 80);

    setStringName("assets/font/Roboto-Regular.ttf");
    setStringSize(36);
    setStringBorder(6);
    setStringBorderRGBA(40, 40, 40, 255);
    igImage* igs[2];//GDI+

    for (i = 0; i < 2; i++)
        igs[i] = g->createIgImage("assets/menu/btn_frame%d.png", i);

    popMenu = new iPopup*[4];
    imgMenuBtn = new iImage*[3];

    for (i = 0; i < 3; i++)
    {
        pop = new iPopup();
        img = new iImage();
        for (j = 0; j < 2; j++)
        {
            g->init(size);
            g->drawIgImage(igs[j], 0, 0, TOP | LEFT);//GDI+

            j == 0 ? setStringRGBA(255, 187, 0, 250) : setStringRGBA(255, 228, 0, 250);
            g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, str[i]);
            tex = g->getTexture();
            img->addObject(tex);
            freeImage(tex);
        }
        img->position = iPointMake(0, 110 * i);
        pop->addObject(img);

        imgMenuBtn[i] = img;

        pop->style = iPopupClash;
        if (i % 2)
            pop->openPoint = iPointMake(-150, devSize.height - 350);
        else
            pop->openPoint = iPointMake(devSize.width, devSize.height - 350);
        pop->closePoint = iPointMake((devSize.width - 150) / 2, devSize.height - 350);

        if (i == 0) pop->methodBefore = drawPopMenuBefore;
        popMenu[i] = pop;
    }

    for (int i = 0; i < 2; i++)
        g->freeIgImage(igs[i]);

    pop = new iPopup();
    img = new iImage();
    size = iSizeMake(1000, 150);
    g->init(size);
    setRGBA(128, 65, 217, 155);
    g->fillRect(0, 0, size.width, size.height, 30);
    
    setStringName("assets/font/Maplestory_Bold.ttf");
    setStringSize(100);
    setStringRGBA(218, 165, 32, 255);
    setStringBorder(10);
    setStringBorderRGBA(40, 40, 40, 255);
    g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "Fantasy War Game");
    tex = g->getTexture();

    img->addObject(tex);
    freeImage(tex);
    pop->addObject(img);

    pop->style = iPopupClash;
    pop->openPoint = iPointMake(devSize.width / 2 - size.width / 2, -200);
    pop->closePoint = iPointMake(devSize.width / 2 - size.width / 2, 100);

    popMenu[3] = pop;

    createPopYesNo();
}

void freePopMenu()
{
    delete imgMenuBtn;

    for (int i = 0; i < 4; i++)
        delete popMenu[i];
    delete popMenu;

    freePopYesNo();
}

#define _shadowDt 3.0f
void drawPopMenuBefore(iPopup* pop, float dt, float rate)
{
    float r = 0.7f;
    if (gm->shadowDt < _shadowDt)
    {
        gm->shadowDt += dt;// / 2;
        r = gm->shadowDt / _shadowDt;
        if (r > 0.7f) r = 0.7f;
    }

    setRGBA(0, 0, 0, 255 * r);
    fillRect(-10, -10, devSize.width + 20, devSize.height + 20);

    resetRGBA();
    for (int i = 0; i < 3; i++)
        imgMenuBtn[i]->setTexObject(i == popMenu[0]->selected);
}

void drawPopMenu(float dt)
{
    resetRGBA();
    for (int i = 0; i < 4; i++)
        popMenu[i]->paint(dt);

    drawPopYesNo(dt);
}

void showPopMenu(bool show)
{
    for (int i = 0; i < 4; i++)
        popMenu[i]->show(show);
}

bool keyPopMenu(iKeyStat stat, iPoint point)
{
    iPopup* pop = popMenu[0];
    if (pop->bShow == false)
        return false;
    if (pop->stat != iPopupProc)
        return true;

    int i, j = -1;

    switch (stat)
    {
        case iKeyStatBegan://case iKeyStatMoved:
            for (i = 0; i < 3; i++)
            {
                if (containPoint(point, imgMenuBtn[i]->touchRect(pop->closePoint)))//, iSizeMake(80, 40))))//터치 영역 늘리기
                {
                    j = i;
                    break;
                }
            }
            if (pop->selected != j)
            {
                if(j != -1)
                    audioPlay(2);
                pop->selected = j;
            }
            break;

        case iKeyStatMoved://iKeyStatEnded:
            i = pop->selected;
            if( i==-1) break;

            if (!containPoint(point, imgMenuBtn[i]->touchRect(pop->closePoint)))//, iSizeMake(80, 40)))==false)//터치 영역 늘리기
                pop->selected = -1;
            break;

        case iKeyStatEnded://iKeyStatBegan:
            if (pop->selected == -1)
                break;

            if (pop->selected == 0)
            {
                playSfxSound(0);
                showPopMenu(false);
                setLoading(gs_proc, freeMenu, loadProc, 2);
            }
            else if (pop->selected == 1)
            {
                playSfxSound(0);
                showPopMenu(false);
                setLoading(gs_intro, freeMenu, loadIntro);
            }
            else// if (popMenu->selected == 2)
            {
                playSfxSound(0);
                showPopYesNo(true, 0);
            }

            pop->selected = -1;
            break;
    }

    return true;
}

