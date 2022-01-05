#include "iVolume.h"

#include "iStd.h"
#include "Loading.h"

iVolume* iv = NULL;
static float musicPs = 200, effectPs = 200;
void volumeBar(float dt);
void volumePopBefore(iPopup* pop, float dt, float rate);

void createPopVolume()
{
    iv = new iVolume;
    iv->pressDt = 0.0f;
    iv->wheel = createImage("assets/icon_wheel.png");

    Texture* tex;
    iImage* img = new iImage();
    iPopup* pop = new iPopup(1);
    iGraphics* g = iGraphics::share();

    iSize size = iSizeMake(540, 340);
    g->init(size);

    size = iSizeMake(500, 300);
    setRGBA(70, 65, 217, 255);
    g->fillRect(20, 20, size.width, size.height, 10);
    setRGBA(146, 146, 146, 255);
    setLineWidth(5);
    g->drawRect(20, 20, size.width, size.height, 10);
    setLineWidth(1);
    
    setStringName("assets/font/BMJUA_ttf.ttf");
    setStringSize(50);
    setStringRGBA(0, 0, 0, 255);
    setStringBorder(3);
    setStringBorderRGBA(146, 146, 146, 255);
    g->drawString(47, 340 / 2 - 50, VCENTER | LEFT, "MUSIC");
    g->drawString(40, 340 / 2 + 50, VCENTER | LEFT, "Effects");

    tex = g->getTexture();

    img->addObject(tex);
    freeImage(tex);
    pop->addObject(img);

    img = new iImage();
    for (int i = 0; i < 2; i++)
    {
        tex = createImage("assets/xbutton%d.png", i);
        img->addObject(tex);
        freeImage(tex);
    }
    img->position = iPointMake(480, 0);
    pop->addObject(img);

    iv->xButton = img;

    pop->style = iPopupZoom;
    if (toGameState == gs_proc)
    {
        pop->openPoint = iPointMake(devSize.width / 2, devSize.height / 2 - 100);
        pop->closePoint = iPointMake(devSize.width / 2 - 540 / 2 + 30, devSize.height / 2 - 340 / 2);
    }
    else
    {
        pop->openPoint = iPointMake(50, 50);
        pop->closePoint = iPointMake(devSize.width / 2 - 540 / 2, devSize.height / 2 - 340 / 2);
    }
    pop->methodBefore = volumePopBefore;
    pop->methodFigure[0] = volumeBar;

    iv->volumes = pop;
    iv->bWheel = iv->bMusic = iv->bEffect = false;

    for (int i = 0; i < 2; i++)
        audioVolume(musicPs / 200, i);

    for (int i = 0; i < 3; i++)
        audioVolume(effectPs / 200, i + 2);

    iv->wBtn = iRectMake(25 - 10, 25 - 10, iv->wheel->width + 20, iv->wheel->height + 20);
    iv->xBtn = iRectMake(iv->volumes->closePoint.x + 480 - 20, iv->volumes->closePoint.y - 20, 60 + 40, 60 + 40);
    iv->mBtn = iRectMake(iv->volumes->closePoint.x + 240 - 20, iv->volumes->closePoint.y + 340 / 2 - 70 - 20, 220 + 40, 40 + 40);
    iv->eBtn = iRectMake(iv->volumes->closePoint.x + 240 - 20, iv->volumes->closePoint.y + 340 / 2 + 30 - 20, 220 + 40, 40 + 40);
}

void freePopVolume()
{
    delete iv->volumes;
    freeImage(iv->wheel);
    delete iv;
}

void volumePopBefore(iPopup* pop, float dt, float rate)
{
    setRGBA(0, 0, 0, 255 * 0.7f);
    fillRect(-10, -10, devSize.width + 20, devSize.height + 20);
    resetRGBA();
}

void volumeBar(float dt)
{
    setRGBA(0, 0, 0, 255);
    fillRect(250, 340 / 2 - 50, 200, 5);
    fillRect(250, 340 / 2 + 50, 200, 5);

    setRGBA(146, 146, 146, 255);
    fillRect(240 + musicPs, 340 / 2 - 70, 20, 40);
    fillRect(240 + effectPs, 340 / 2 + 30, 20, 40);

    resetRGBA();
}

void drawPopVolume(float dt)
{
    float degree = 20 * iv->pressDt;
    if (iv->volumes->bShow)
        iv->pressDt += dt;

    int n = iv->volumes->selected;
    if (n == -1) n = 0;// -1 => iPopup 255 => iImage frame
    iv->xButton->setTexObject(n);
    iv->volumes->paint(dt);

    setRGBA(146, 146, 146, 255);

    drawImage(iv->wheel, 50, 50, VCENTER | HCENTER,
        0, 0, iv->wheel->width, iv->wheel->height, 1, 1, 2, degree);

    resetRGBA();
}

void volumePopup(float dt)//When inactive wheel button ... drawpop
{
    if(iv->volumes->bShow)
    {
        iv->xButton->setTexObject(iv->volumes->selected);
        iv->volumes->paint(dt);
    }
}

void showPopVolume(bool show)
{
    iv->volumes->show(show);
}

void volumeCollision(iPoint point)
{
    if (containPoint(point, iv->xBtn))
        iv->volumes->selected = 1;
    else
        iv->volumes->selected = 0;

    if (containPoint(point, iv->mBtn))
        iv->bMusic = true;

    if (containPoint(point, iv->eBtn))
        iv->bEffect = true;
}

void volumeControl(iPoint point)
{
    if (iv->bMusic)
    {
        musicPs = point.x - iv->volumes->closePoint.x - 240;
        if (musicPs > 200)      musicPs = 200;
        else if (musicPs < 0)   musicPs = 0;

        for (int i = 0; i < 2; i++)
            audioVolume(musicPs / 200, i);
    }
    else if (iv->bEffect)
    {
        effectPs = point.x - iv->volumes->closePoint.x - 240;
        if (effectPs > 200)      effectPs = 200;
        else if (effectPs < 0)   effectPs = 0;

        for (int i = 0; i < 3; i++)
            audioVolume(effectPs / 200, i + 2);
    }
}

bool keyPopVolume(iKeyStat stat, iPoint point)
{
    switch (stat)
    {
        case iKeyStatBegan:
        {
            if (gameState == gs_intro ||
                gameState == gs_menu)
            {
                if (containPoint(point, iv->wBtn))
                    iv->bWheel = true;
                else
                    iv->bWheel = false;
            }

            if(iv->volumes->bShow)
                volumeCollision(point);

            break;
        }
        case iKeyStatMoved:
        {
            if (gameState == gs_intro ||
                gameState == gs_menu)
            {
                if (containPoint(point, iv->wBtn))
                    iv->bWheel = true;
                else
                    iv->bWheel = false;
            }

            if(iv->volumes->bShow)
            {
                volumeCollision(point);
                volumeControl(point);
            }

            break;
        }
        case iKeyStatEnded:
        {
            if(iv->bWheel)
            {
                audioPlay(2);
                iv->volumes->bShow ? showPopVolume(false) : showPopVolume(true);
            }

            if (iv->volumes->bShow && containPoint(point, iv->xBtn))
            {
                audioPlay(2);
                showPopVolume(false);
            }
            iv->volumes->selected = 0;
            iv->bWheel = iv->bMusic = iv->bEffect = false;
            break;
        }
    }

    if (iv->volumes->bShow)
        return true;
    else
        return false;
}

