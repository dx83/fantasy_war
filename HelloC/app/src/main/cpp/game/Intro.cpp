#include "Intro.h"

#include "iStd.h"
#include "Loading.h"
#include "Menu.h"
#include "Sound.h"

Intro* it;
void pptChange(int page);
//void pptDraw(float dt);

#if CHECK_TEXTURE
int introTexNum;
#endif
void loadIntro()
{
#if CHECK_TEXTURE
    introTexNum = texture_num;
#endif
    setStringName("assets/font/BMJUA_ttf.ttf");

    it = new Intro;

    it->ppt = new iPage();

    it->ppt->init(true, 2, iRectMake(0, 0, devSize.width, devSize.height),
        pptChange, NULL, NULL, NULL);
    
    it->contents = new Texture*[3];

    for (int i = 0; i < 2; i++)
        it->contents[i] = createImage("assets/intro/slide%d.PNG", i);

    //1136 640 960 720
    it->rw = devSize.width / it->contents[0]->width;
    it->rh = devSize.height / it->contents[0]->height;

    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(200, 150);
    size = iSizeMake(400, 80);
    g->init(size);
    setRGBA(159, 159, 159, 255);
    g->fillRect(0, 0, size.width, size.height, 40);
    setStringSize(55);//
    setStringRGBA(0, 0, 0, 255);
    setStringBorder(5);
    setStringBorderRGBA(255, 255, 255, 255);
    g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "In to the Game");
    it->contents[2] = g->getTexture();
    it->pressDt = 0.0f;

    it->onTouch = false;
    it->onClick = false;

    //volumes
    createPopVolume();
    showPopVolume(false);
}

void freeIntro()
{
    delete it->ppt;

    for (int i = 0; i < 3; i++)
        freeImage(it->contents[i]);
    delete it->contents;

    delete it;
    freePopVolume();
#if CHECK_TEXTURE
    loge("INTRO ==== prev(%d) curr(%d)", introTexNum, texture_num);
#endif
}

void pptChange(int page)
{
    //printf("changed page = %d\n", page);
    //loge("changed page = %d\n", page);
}

void drawIntro(float dt)
{
    clearRect(0.5f, 0.5f, 0.5f, 1.0f);
    resetRGBA();

    it->ppt->paint(dt, iPointZero);

    iPoint p = it->ppt->rt.origin + it->ppt->off;
    Texture** t = it->contents;

    drawImage(t[0], p.x, p.y, TOP | LEFT,
              0, 0, t[0]->width, t[0]->height, it->rw, it->rh, 2, 0);
    drawImage(t[1], p.x + devSize.width, p.y, TOP | LEFT,
              0, 0, t[1]->width, t[1]->height, it->rw, it->rh, 2, 0);

    //resetRGBA();

    if (!it->onClick)
    {
        uint8 ca;
        it->pressDt += dt;
        float rate = _sin(180 * ((it->pressDt + 1) / 2));
        if (rate < 0)
            rate *= -1;
        ca = (uint8)(255 * rate);
        setAlpha(ca);
    }
    else
    {
        setRGBA(255, 54, 54, 255);
    }

    drawImage(t[2],  p.x + devSize.width * 2 - 25 - it->contents[2]->width,
                         p.y + 20, TOP | LEFT);// TOP | RIGHT

    drawPopVolume(dt);

    resetRGBA();
}

void keyIntro(iKeyStat stat, iPoint point)
{
    if( keyPopVolume(stat, point) )
        return;

    iPoint p = it->ppt->rt.origin + it->ppt->off;
    iRect rt;
    rt = iRectMake(p.x + devSize.width * 2 - 25 - it->contents[2]->width, p.y + 20,
                   it->contents[2]->width, it->contents[2]->height);

    switch (stat)
    {
#if (OS==OS_WINDOWS)
    case iKeyStatBegan:
        if (containPoint(point, rt))
            it->onClick = true;
        break;
    case iKeyStatMoved:
        containPoint(point, rt) ? it->onTouch = true : it->onTouch = false;
        break;
#elif (OS==OS_ANDROID)
    case iKeyStatBegan:
    case iKeyStatMoved:
        if (containPoint(point, rt))
            it->onClick = true;
        else
            it->onClick = false;
        break;
#endif

    case iKeyStatEnded:
        if (it->onClick)
            setLoading(gs_menu, freeIntro, loadMenu);
        it->onClick = false;
        break;
    }

    it->ppt->key(stat, point);
}

