#include "LoadMap.h"

#include "iStd.h"
#include "DrawCounters.h"
#include "ProcUI_Commander.h"
#include "ProcUI_MapKey.h"
#include "ProcUI_MiniMap.h"
#include "ProcUI_RegimentInfo.h"
#include "RegimentAction.h"

static LoadMap* lm;
int currentIndex;

void drawMapBorder(float dt);
void asdwMap();

//Tile w96 h80 w 55 h 25
void createHexMap()
{
    currentIndex = -1;

    lm = new LoadMap;
    lm->totalW = 55, lm->totalH = 25;
    lm->hMax = lm->totalW * lm->totalH;
    lm->off = new mapOffSet;
    lm->off->mX = 48, lm->off->mY = 80;
    
    lm->tex = new Texture*[2];
    lm->tex[0] = createImage("assets/map/game_map_full.png");
    lm->prev = iPointZero;
    lm->off->max = iPointMake(0, 0);
    lm->off->min = iPointMake(devSize.width, devSize.height) - iPointMake(lm->tex[0]->width - 96, lm->tex[0]->height - 120);
    lm->off->curr = iPointZero;//lm->off->min / 2;
    lm->bMove = false;

    lm->hexs = new Hexagon[lm->hMax];
    for (int i = 0; i < lm->hMax; i++)
    {
        Hexagon* h = &lm->hexs[i];
        h->rt.size.width = 96;
        h->rt.size.height = 80;
        h->exHex = true;
        for (int j = 0; j < 6; j++)
            h->way[j] = 0;
        h->terrain = -1;
        h->move = 0;
        h->landmark = -1;
    }

    for (int j = 0; j < lm->totalH; j++)
    {
        for (int i = 0; i < lm->totalW; i++)
        {
            int idx = lm->totalW * j + i;
            Hexagon* h = &lm->hexs[idx];
            if (i % 2)
            {
                h->point = iPointMake(72 * i, 80 * j);
                h->rt.origin.x = h->point.x - 48;
                h->rt.origin.y = h->point.y - 40;
                h->xIdx = i;
                h->yIdx = j;
                if (j > 0 && j < lm->totalH - 1 && i != 0 && i != lm->totalW - 1)
                {   // 경계부분 제외하고(totalH-2 홀수 부분은 정상헥스)
                    h->exHex = false;
                    h->way[0] = idx - lm->totalW;
                    h->way[1] = idx - (lm->totalW - 1);
                    h->way[2] = idx + 1;
                    h->way[3] = idx + lm->totalW;
                    h->way[4] = idx - 1;
                    h->way[5] = idx - (lm->totalW + 1);
                }
            }
            else
            {
                h->point = iPointMake(72 * i, 80 * j + 40);
                h->rt.origin.x = h->point.x - 48;
                h->rt.origin.y = h->point.y - 40;
                h->xIdx = i;
                h->yIdx = j;
                if (j > 0 && j < lm->totalH - 2 && i != 0 && i != lm->totalW - 1)
                {   // 경계부분 제외하고(totalH-2 짝수 부분은 제외헥스)
                    h->exHex = false;
                    h->way[0] = idx - lm->totalW;
                    h->way[1] = idx + 1;
                    h->way[2] = idx + lm->totalW + 1;
                    h->way[3] = idx + lm->totalW;
                    h->way[4] = idx + lm->totalW - 1;
                    h->way[5] = idx - 1;
                }
            }
        }
    }
    //loge("Hexes Count : %d", lm->hMax);

#ifdef PointText
    iGraphics* g = iGraphics::share();
    lm->pText = new Texture * [lm->hMax];

    setStringName("바탕");
    setStringSize(20);
    setStringRGBA(0, 0, 0, 255);
    setStringBorder(1);
    setStringBorderRGBA(0, 0, 0, 255);
    setStringStyle(1);

    lm->size = iSizeMake(100, 30);
    for (int i = 0; i < lm->hMax; i++)
    {
        if (lm->hexs[i]->exHex) continue;
        g->init(lm->size);
#if 0
        tx = hex[i]->xIdx;
        ty = hex[i]->yIdx;
        g->drawString(0, 0, TOP | LEFT, "(%02d,%02d)", tx, ty);
#else
        g->drawString(0, 0, TOP | LEFT, "[%0004d]", i);
#endif
        lm->pText[i] = g->getTexture();
    }
#endif//PointText

    // Map Border
    lm->sp = iPointZero; lm->ep = iPointZero;
    lm->tex[1] = createImage("assets/map/map_knight.png");
    //worldmap border line
    lm->widthOff = lm->tex[0]->width - lm->off->mX - 76;
    lm->heightOff = lm->tex[0]->height - lm->off->mY - 120;
    //worldmap corner deco
    lm->cornerW = lm->tex[0]->width - lm->tex[1]->width + lm->off->mX * 0.58;
    lm->cornerH = lm->tex[0]->height - lm->tex[1]->height + lm->off->mY * 0.90;
    // Fog
    lm->texFog = createImage("assets/map/game_map_fog.png");
    lm->fogOn = true;
    lm->fogHex = true;
}

void freeHexMap()
{
#ifdef PointText
    delete lm->pText;
#endif//PointText
    delete lm->hexs;
    delete lm->off;
    for (int i = 0; i < 2; i++)
        freeImage(lm->tex[i]);
    delete lm->tex;
    freeImage(lm->texFog);
    delete lm;
}

void drawHexMap(float dt)
{
    Texture* t = lm->tex[0];
    drawImage(t, lm->off->curr.x, lm->off->curr.y, TOP | LEFT,
        lm->off->mX, lm->off->mY, t->width, t->height, 1, 1, 2, 0);
    drawMapBorder(dt);

    if (lm->fogOn)
    {
        setRGBA(0, 0, 0, 245);
        drawImage(lm->texFog, lm->off->curr.x, lm->off->curr.y - 120, TOP | LEFT,
                  0, 0, lm->texFog->width, lm->texFog->height, 2, 2, 2, 0);
    }

#ifdef PointText
    for (int i = 0; i < 4; i++)
        setRGBA(255, 0, 25, 255, i);
    for (int j = 0; j < lm->totalH; j++)
    {
        for (int i = 0; i < lm->totalW; i++)
        {
            int idx = lm->totalW * j + i;
            if (lm->hexs[idx]->exHex) continue;
    
            Hexagon* h = lm->hexs[idx];
            iRect rt = iRectMake(h->point.x + lm->off->curr.x - 2, h->point.y + lm->off->curr.y - 2, 4, 4);
            fillRect(rt);
            drawImage(lm->pText[idx], rt.origin.x - 30, rt.origin.y + 10, TOP | LEFT);
        }
    }
    //int j = 23;
    //for (int i = 0; i < lm->totalW; i++)
    //{
    //    int idx = lm->totalW * j + i;
    //    if (lm->hexs[idx]->exHex) continue;
    //
    //    Hexagon* h = lm->hexs[idx];
    //    iRect rt = iRectMake(h->point.x + lm->off->curr.x - 2, h->point.y + lm->off->curr.y - 2, 4, 4);
    //    fillRect(rt);
    //    drawImage(lm->pText[idx], rt.origin.x - 30, rt.origin.y + 10, TOP | LEFT);
    //}
#endif

#ifdef DEVELOP
    asdwMap();
#endif//DEVELOP(iStd.h)
    if (getKeyDown() & keysF1)
        lm->fogOn = !lm->fogOn;
    resetRGBA();
}

void drawMapBorder(float dt)
{
    int i, lineW[3];
    bool smallView;
    if (viewport.size.width > 1300.f)
    {
        smallView = false;
        lineW[0] = 3; lineW[1] = 4; lineW[2] = 2;
    }
    else if (viewport.size.width > 1000.f)
    {
        smallView = false;
        lineW[0] = 2; lineW[1] = 3; lineW[2] = 1;
    }
    else if (viewport.size.width > 700.f)
    {
        smallView = false;
        lineW[0] = 1; lineW[1] = 2; lineW[2] = 1;
    }
    else
    {
        smallView = true;
        lineW[0] = 1; lineW[1] = 2; lineW[2] = 0;
    }

    for (i = 0; i < 6; i++)// L,R
    {
        if (smallView && i % 3 == 2)  continue;
        if (i == 1 || i == 4)
            setRGBA(184, 134, 11, 255);
        else
            setRGBA(218, 165, 32, 255);
        if (i < 3)
        {
            setLineWidth(lineW[(i % 3)]);
            lm->sp = iPointMake(lm->off->curr.x + 6 + (i * 7), lm->off->curr.y + 80);
            lm->ep = iPointMake(lm->off->curr.x + 6 + (i * 7), lm->off->curr.y + lm->heightOff);
            drawLine(lm->sp, lm->ep);//LEFT
            lm->sp = iPointMake(lm->off->curr.x + 20, lm->off->curr.y + 10 + (i * 8));
            lm->ep = iPointMake(lm->off->curr.x + lm->widthOff, lm->off->curr.y + 10 + (i * 8));
            drawLine(lm->sp, lm->ep);//TOP
        }
        else
        {
            setLineWidth(lineW[5 - i]);
            lm->sp = iPointMake(lm->off->curr.x + 6 + (i * 7) + lm->widthOff - 20, lm->off->curr.y + 80);
            lm->ep = iPointMake(lm->off->curr.x + 6 + (i * 7) + lm->widthOff - 20, lm->off->curr.y + lm->heightOff);
            drawLine(lm->sp, lm->ep);//RIGHT
            lm->sp = iPointMake(lm->off->curr.x + 20, lm->off->curr.y + 10 + (i * 8) + lm->heightOff + 20);
            lm->ep = iPointMake(lm->off->curr.x + lm->widthOff, lm->off->curr.y + 10 + (i * 8) + lm->heightOff + 20);
            drawLine(lm->sp, lm->ep);//BOTTOM
        }
    }
    
    Texture* corner = lm->tex[1];
    setRGBA(25, 25, 25, 255);
    drawImage(corner, lm->off->curr.x, lm->off->curr.y, TOP | LEFT,
        0, 0, corner->width, corner->height, 0.3f, 0.3f, 2, 0);//TL
    drawImage(corner, lm->off->curr.x + lm->cornerW, lm->off->curr.y + lm->cornerH, TOP | LEFT,
        0, 0, corner->width, corner->height, 0.3f, 0.3f, 1, 180, REVERSE_HEIGHT);//BR
    setRGBA(238, 232, 170, 255);
    drawImage(corner, lm->off->curr.x + lm->cornerW, lm->off->curr.y, TOP | LEFT,
        0, 0, corner->width, corner->height, 0.3f, 0.3f, 1, 180);//TR
    drawImage(corner, lm->off->curr.x, lm->off->curr.y + lm->cornerH, TOP | LEFT,
        0, 0, corner->width, corner->height, 0.3f, 0.3f, 2, 0, REVERSE_HEIGHT);//BL
    
    resetRGBA();
}

bool keyHexMap(iKeyStat stat, iPoint point)
{
    if (allPopMoving())
        return false;

    switch (stat)
    {
#if (OS==OS_WINDOWS)
    case iKeyStatBegan:
        //printf("LoadMap began(%f,%f)\n", point.x, point.y);
        lm->bMove = true;
        lm->prev = point;
        lm->bClick = true;
        break;
    case iKeyStatMoved:
        if (lm->bMove)
        {
            iPoint mp = point - lm->prev;
            if (iPointLength(mp) > 3)
            {
                lm->off->curr += mp;
                lm->prev = point;
                restrictMap(lm->off->curr, lm->off->min, lm->off->max);
                lm->bClick = false;
                break;
            }
        }
        else
        {
            int shortest = 0;
            float currLength = 10000000.f;

            for (int i = 0; i < lm->hMax; i++)
            {
                Hexagon* h = &lm->hexs[i];
                iRect rt = iRectMake(h->rt.origin.x + lm->off->curr.x, h->rt.origin.y + lm->off->curr.y,
                    h->rt.size.width, h->rt.size.height);
                if (containPoint(point, rt))
                {
                    float length;
                    int idx;
                    for (int j = 0; j < 7; j++)
                    {
                        if (j == 0)
                        {
                            idx = i;
                            length = iPointDistance(h->point + lm->off->curr, point);
                        }
                        else
                        {
                            idx = h->way[j - 1];
                            length = iPointDistance(lm->hexs[idx].point + lm->off->curr, point);
                        }
                        if (length < 40)
                        {
                            shortest = idx;
                            currLength = length;
                            break;
                        }
                        else if (currLength > length)
                        {
                            shortest = idx;
                            currLength = length;
                        }
                    }
                    break;
                }
            }
            currentIndex = shortest;
        }
        break;

    case iKeyStatEnded:
        if (lm->bClick)//only click
        {
# if 1
            int shortest = 0;
            float currLength = 10000000.f;

            for (int i = 0; i < lm->hMax; i++)
            {
                Hexagon* h = &lm->hexs[i];
                iRect rt = iRectMake(   h->rt.origin.x + lm->off->curr.x, h->rt.origin.y + lm->off->curr.y,
                                        h->rt.size.width, h->rt.size.height);
                if (containPoint(point, rt))
                {
                    float length;
                    int idx;
                    for (int j = 0; j < 7; j++)
                    {
                        if (j == 0)
                        {
                            idx = i;
                            length = iPointDistance(h->point + lm->off->curr, point);
                        }
                        else
                        {
                            idx = h->way[j - 1];
                            length = iPointDistance(lm->hexs[idx].point + lm->off->curr, point);
                        }
                        if (length < 40)
                        {
                            shortest = idx;
                            currLength = length;
                            break;
                        }
                        else if(currLength > length)
                        {
                            shortest = idx;
                            currLength = length;
                        }
                    }
                    break;
                }
            }
            //확인용
            //if(lm->hexs[shortest]->exHex)
            //    printf("[%0004d] MAP OUT\n", shortest);
            //else
            //{
            //    printf("[%0004d](%02d,%02d) %.3f\n", shortest, lm->hexs[shortest]->xIdx, lm->hexs[shortest]->yIdx, currLength);
            //    int d[6];
            //    for (int i = 0; i < 6; i++)
            //        d[i] = lm->hexs[shortest]->way[i];
            //    lm->hexs[d[0]]->exHex ? printf("nth:NULL ") : printf("nth:%d ", d[0]);
            //    lm->hexs[d[1]]->exHex ? printf("en:NULL ") : printf("en:%d ", d[1]);
            //    lm->hexs[d[2]]->exHex ? printf("es:NULL ") : printf("es:%d ", d[2]);
            //    lm->hexs[d[3]]->exHex ? printf("sth:NULL ") : printf("sth:%d ", d[3]);
            //    lm->hexs[d[4]]->exHex ? printf("ws:NULL ") : printf("ws:%d ", d[4]);
            //    lm->hexs[d[5]]->exHex ? printf("wn:NULL ") : printf("wn:%d ", d[5]);
            //    printf("\n");
            //}
            Hexagon* h = lm->hexs;
            printf("[%0004d](%02d,%02d) %.3f\n", shortest, h[shortest].xIdx, h[shortest].yIdx, currLength);
            int d[6];
            for (int i = 0; i < 6; i++)
                d[i] = h[shortest].way[i];
            h[d[0]].exHex ? printf("nth:NULL ") : printf("nth:%d ", d[0]);
            h[d[1]].exHex ? printf("en:NULL ") : printf("en:%d ", d[1]);
            h[d[2]].exHex ? printf("es:NULL ") : printf("es:%d ", d[2]);
            h[d[3]].exHex ? printf("sth:NULL ") : printf("sth:%d ", d[3]);
            h[d[4]].exHex ? printf("ws:NULL ") : printf("ws:%d ", d[4]);
            h[d[5]].exHex ? printf("wn:NULL ") : printf("wn:%d ", d[5]);
            printf("\n");
            //printf("point: %f, %f x: %f y: %f\n", lm->hexs[shortest]->point.x, lm->hexs[shortest]->point.y, 
            //    lm->hexs[shortest]->rt.origin.x, lm->hexs[shortest]->rt.origin.y);
            //printf("%.3f\n", iPointDistance(hex[108].point + _offSet->curr, hex[109].point + _offSet->curr));//점:점
#endif
        }

        lm->bMove = false;
        lm->bClick = false;
        break;
#elif (OS==OS_ANDROID)
    case iKeyStatBegan:
        //printf("LoadMap began(%f,%f)\n", point.x, point.y);
        lm->prev = point;
        lm->bMove = false;
        break;

    case iKeyStatMoved:
    {
        iPoint mp = point - lm->prev;

        if (iPointLength(mp) > 10)
        {
            lm->off->curr += mp;
            lm->prev = point;
            restrictMap(lm->off->curr, lm->off->min, lm->off->max);
            lm->bMove = true;
            return true;
        }
        break;
    }

    case iKeyStatEnded:
        if(lm->bMove == false)//only click
        {
            int shortest = 0;
            float currLength = 10000000.f;

            for (int i = 0; i < lm->hMax; i++)
            {
                Hexagon* h = &lm->hexs[i];
                iRect rt = iRectMake(h->rt.origin.x + lm->off->curr.x, h->rt.origin.y + lm->off->curr.y,
                                        h->rt.size.width, h->rt.size.height);
                if (containPoint(point, rt))
                {
                    float length;
                    int idx;
                    for (int j = 0; j < 7; j++)
                    {
                        if (j == 0)
                        {
                            idx = i;
                            length = iPointDistance(h->point + lm->off->curr, point);
                        }
                        else
                        {
                            idx = h->way[j - 1];
                            length = iPointDistance(lm->hexs[idx].point + lm->off->curr, point);
                        }
                        if (length < 40)
                        {
                            shortest = idx;
                            currLength = length;
                            break;
                        }
                        else if(currLength > length)
                        {
                            shortest = idx;
                            currLength = length;
                        }
                    }
                    break;
                }
            }
            currentIndex = shortest;
            if ( lm->fogOn )
                fogStatus(currentIndex);
            //loge("currentIndex %d", currentIndex);
            //loge("%d %d", _dc->focusIdx, currentIndex);

            if(actRegimentAction())//commander -> move btn 이 켜져있는 경우
                break;

            if(focusCommander() == false)//&& _dc->focusIdx != -1)
                _dc->focusIdx = -1;

            int armyIdx = lm->hexs[currentIndex].regimentIdx;
            if (armyIdx != -1)
                showCommander(true);
            else if (_dc->focusIdx == -1)
                showCommander(false);
        }
        break;
#endif
    }

    return false;
}

bool allPopMoving()
{
    return movingMapkey()   ||
           movingMinimap()  ||
           movingCommander();
}

void asdwMap()//개발자용
{
    int key = getKeyStat();
    if (!key)
        return;

    if (key & keysA)            lm->off->curr += iPointMake(10, 0);
    else if (key & keysD)       lm->off->curr += iPointMake(-10, 0);

    if (key & keysW)            lm->off->curr += iPointMake(0, 10);
    else if (key & keysS)       lm->off->curr += iPointMake(0, -10);

    restrictMap(lm->off->curr, lm->off->min, lm->off->max);
}

iPoint& restrictMap(iPoint& off, iPoint min, iPoint max, float mx, float my)
{
    if (off.x < min.x)
        off.x = min.x;
    else if (off.x + mx > max.x)
        off.x = max.x - mx;

    if (off.y < min.y)
        off.y = min.y;
    else if (off.y + my > max.y)
        off.y = max.y - my;

    return off;
}

void fogOnOff()
{
    lm->fogOn = !lm->fogOn;
    if ( lm->fogOn == false )
        lm->fogHex = false;
}

bool fogStatus(int index)
{
    if ( lm->fogOn == false)
        return lm->fogHex = false;
    else if ( index == -1)
        return lm->fogHex = true;
    else if ( index != -2 )
    {
        int width = 518, w = 17, h = 13, off = 55;//518, 573 ~
        for (int j = 0; j < h;j++)
        {
            if ( index > width + (j * off) && index < width + w + (j * off))
                return lm->fogHex = false;
        }

        return lm->fogHex = true;
    }

    return lm->fogHex;// index == -2, 현재상태 출력
}

mapOffSet* getMapOffSet()
{
    return lm->off;
}

void setMapOffCurr(iPoint point)
{
    lm->off->curr = point;
}

Hexagon* mapKeyInHex()
{
    return lm->hexs;
}

Hexagon* _hexgon = NULL;
Hexagon* getHexInfo()   // 해제는 프로그램 종료시 자동 해지
{
    if (_hexgon)
        delete _hexgon;
    _hexgon = new Hexagon[lm->hMax];
    memcpy(_hexgon, lm->hexs, sizeof(Hexagon) * lm->hMax);
    return _hexgon;
}

int getRegimentIndex(int hexIdx)
{
    return lm->hexs[hexIdx].regimentIdx;
}

void setRegimentIndex(int hexIdx, int idx)
{
    lm->hexs[hexIdx].regimentIdx = idx;
}

