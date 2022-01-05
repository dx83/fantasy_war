#include "DrawCounters.h"

#include "iStd.h"

#include "LoadMap.h"
#include "RegimentMgr.h"
#include "ProcUI_Commander.h"
#include "ProcUI_RegimentInfo.h"
#include "RegimentAction.h"

CounterTexture* counterTex;
DrawCounters* _dc;
void drawRegimentTitle(iPoint off, int idx);

void createCounters()
{
    int i, j;
    counterTex = new CounterTexture;
    counterTex->m = new Texture*[12];
    counterTex->w = new Texture*[12];

    for (i = 0; i < 12; i++)
    {
        counterTex->m[i] = createImage("assets/map/counter_map_%d.png", i);
        counterTex->w[i] = createImage("assets/map/counter_war_%d.png", i);
    }
    // 0sadron 1dun boldor 2vehirn 3mastema
    // 4aindra 5hydra meiny 6karasad 7shantanu
    counterTex->sp = new Texture**[8];
    counterTex->bp = new Texture**[8];
    counterTex->sh = new Texture*[8];
    counterTex->bh = new Texture*[8];
    for (i = 0; i < 8; i++)
    {
        counterTex->sp[i] = new Texture*[4];
        counterTex->bp[i] = new Texture*[4];
        for (j = 0; j < 4; j++)
        {
            counterTex->sp[i][j] = createImage("assets/map/pattern_%d_s%d.png", i, j);
            counterTex->bp[i][j] = createImage("assets/map/pattern_%d_b%d.png", i, j);
        }
        counterTex->sh[i] = createImage("assets/map/heraldry_s%d.png", i);
        counterTex->bh[i] = createImage("assets/map/heraldry_b%d.png", i);
    }
    counterTex->ardr = false;

    setStringName("assets/font/HStoad.ttf");
    setStringSize(11);
    setStringRGBA(0, 0, 0, 255);
    setStringBorder(1);
    setStringBorderRGBA(100, 100, 100, 255);
    setStringStyle(0);
    
    counterTex->tMove = new Texture*[10];
    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(30, 24);
    for (i = 0; i < 10; i++)
    {
        g->init(size);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%d", i);
        counterTex->tMove[i] = g->getTexture();
    }
    
    setStringRGBA(186, 43, 43, 255);
    setStringBorderRGBA(255, 167, 167, 255);
    counterTex->tMnum = new Texture * [102];
    for (int i = 0; i < 102; i++)
    {
        g->init(size);
        if (i == 51)
        {
            setStringRGBA(49, 98, 199, 255);
            setStringBorderRGBA(178, 204, 255, 255);
        }

        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%d", i % 51);
        counterTex->tMnum[i] = g->getTexture();
    }

    setStringName("assets/font/RixYeoljeongdo_Regular.ttf");
    setStringSize(30);
    setStringRGBA(255, 255, 0, 255);
    setStringBorder(5);
    setStringBorderRGBA(0, 0, 0, 200);
    setStringStyle(0);

    counterTex->tWnum = new Texture*[102];
    size = iSizeMake(67, 48);
    for (int i = 0; i < 102; i++)
    {
        g->init(size);
        if(i == 51) setStringRGBA(0, 255, 0, 255);

        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%d", i % 51);
        counterTex->tWnum[i] = g->getTexture();
    }

    counterTex->selectHex = createImage("assets/map/select_hex_border.png");

    counterTex->title = new Texture*[2];
    for (i = 0; i < 2; i++)
        counterTex->title[i] = createImage("assets/map/army_title_round%d.png", i);

    counterTex->mark = new Texture*[4];
    for (i = 0; i < 3; i++)
        counterTex->mark[i] = createImage("assets/map/hero_rank_%d.png", i);

    counterTex->mark[3] = createImage("assets/map/mark_x.png");

    _dc = new DrawCounters;
    _dc->focusIdx = -1;

    _dc->focusRgIdx = -1;
    _dc->currRgIdx = -1;
    _dc->bClickOBJ = false;

    _dc->hcenter = (devSize.width - 120) / 2;
    _dc->battleW = devSize.width - 120;
}

void freeCounters()
{
    int i, j;

    for (i = 0; i < 12; i++)
    {
        freeImage(counterTex->m[i]);
        freeImage(counterTex->w[i]);
    }
    delete counterTex->m;
    delete counterTex->w;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 4; j++)
        {
            freeImage(counterTex->sp[i][j]);
            freeImage(counterTex->bp[i][j]);
        }
        delete counterTex->sp[i];
        delete counterTex->bp[i];

        freeImage(counterTex->sh[i]);
        freeImage(counterTex->bh[i]);
    }
    delete counterTex->sp;
    delete counterTex->bp;
    delete counterTex->sh;
    delete counterTex->bh;

    for (i = 0; i < 10; i++)
        freeImage(counterTex->tMove[i]);
    delete counterTex->tMove;

    for (i = 0; i < 102; i++)
        freeImage(counterTex->tMnum[i]);
    delete counterTex->tMnum;

    for (i = 0; i < 102; i++)
        freeImage(counterTex->tWnum[i]);
    delete counterTex->tWnum;

    freeImage(counterTex->selectHex);
    for (i = 0; i < 2; i++)
        freeImage(counterTex->title[i]);
    delete counterTex->title;
    for (i = 0; i < 4; i++)
        freeImage(counterTex->mark[i]);
    delete counterTex->mark;

    delete counterTex;

    delete _dc;
}

void drawCounters(float dt)
{
    drawMapCounters(dt);
    //drawWarCounters(dt);

    drawRegimentAction(dt);
    drawSelectHex(dt);
#if 0
    if (getKeyStat() & keysSpace)
        counterTex->ardr = true;
    else
        counterTex->ardr = false;
#endif
}

void drawMapCounters(float dt)
{
    mapOffSet* offSet = getMapOffSet();

    int max, power;
    max = regimentsTotalCount();
    for (int i = 0; i < max; i++)
    {
        RegimentInfo* rg = regimentAtIndex(i);

        if (counterTex->ardr)
            rg->ally ? power = rg->dr + 51 : power = rg->ar;
        else
            rg->ally ? power = rg->ar : power = rg->dr + 51;

        for (int j = 0; j < rg->count; j++)
        {
            int type = rg->army[rg->count - 1 - j]->type;
            Texture* texPattern = counterTex->sp[rg->army[0]->nation][rg->army[0]->pattern];

            float typeX = 0, typeY = 0, patternX = 0, patternY = 0;
            float powerX = 0, powerY = 0, moveX = 0, moveY = 0;

            if (rg->count == 1)
            {
                typeX = offSet->curr.x, typeY = offSet->curr.y;
                patternX = offSet->curr.x + 5, patternY = offSet->curr.y - 7;
                powerX = offSet->curr.x + 5, powerY = offSet->curr.y + 7;
                moveX = offSet->curr.x + 27, moveY = offSet->curr.y + 7;
            }
            else if (rg->count == 2)
            {
                typeX = offSet->curr.x + (j * 10) - 5, typeY = offSet->curr.y - (j * 10) + 5;
                patternX = offSet->curr.x + 5 + (j * 10) - 5, patternY = offSet->curr.y - 7 - (j * 10) + 5;
                powerX = offSet->curr.x + 10, powerY = offSet->curr.y + 2;
                moveX = offSet->curr.x + 32, moveY = offSet->curr.y + 2;
            }
            else if (rg->count == 3)
            {
                typeX = offSet->curr.x + (j * 8) - 8, typeY = offSet->curr.y - (j * 8) + 8;
                patternX = offSet->curr.x + 5 + (j * 8) - 8, patternY = offSet->curr.y - 7 - (j * 8) + 8;
                powerX = offSet->curr.x + 13, powerY = offSet->curr.y;
                moveX = offSet->curr.x + 35, moveY = offSet->curr.y;
            }
            else if (rg->count == 4)
            {
                typeX = offSet->curr.x + (j * 6) - 9, typeY = offSet->curr.y - (j * 6) + 9;
                patternX = offSet->curr.x + 5 + (j * 6) - 9, patternY = offSet->curr.y - 7 - (j * 6) + 9;
                powerX = offSet->curr.x + 15, powerY = offSet->curr.y - 2;
                moveX = offSet->curr.x + 37, moveY = offSet->curr.y - 2;
            }

            drawImage(counterTex->m[type], rg->ps.x + typeX, rg->ps.y + typeY, VCENTER | HCENTER);
            if (j == rg->count - 1)
            {
                drawImage(texPattern, rg->ps.x + patternX, rg->ps.y + patternY, VCENTER | HCENTER);

                drawImage(counterTex->tMnum[power], rg->ps.x + powerX, rg->ps.y + powerY, TOP | RIGHT);
                drawImage(counterTex->tMove[rg->mv], rg->ps.x + moveX, rg->ps.y + moveY, TOP | RIGHT);
            }
        }
    }

    resetRGBA();
}

void drawRegimentTitle(iPoint off, int idx)
{
    int armyIdx = getRegimentIndex(idx);
    if (armyIdx != -1)
    {
        RegimentInfo* rg = regimentAtIndex(armyIdx);

        Texture* titleImg = counterTex->title[rg->ally];
        drawImage(titleImg, rg->ps.x + off.x, rg->ps.y + off.y - 52, VCENTER | HCENTER,
            0, 0, titleImg->width, titleImg->height, 0.5f, 0.4f, 2, 0);

        Texture* mark = counterTex->mark[rg->gi->mark];
        drawImage(mark, rg->ps.x + off.x, rg->ps.y + off.y - 67, BOTTOM | HCENTER);

        Texture* name = regimentText->heroNameTitle[rg->gi->idx];
        drawImage(name, rg->ps.x + off.x, rg->ps.y + off.y - 52, VCENTER | HCENTER);
        // commader
        _dc->ally = rg->ally;
        _dc->heroName = name;
    }
}

void drawSelectHex(float dt)
{
    int idx;
    mapOffSet* off = getMapOffSet();
    Hexagon* hex = getHexInfo();

    _dc->focusIdx == -1 ? idx = currentIndex : idx = _dc->focusIdx;
    if ( idx == -1 ) return;

    drawImage(counterTex->selectHex, hex[idx].point.x + off->curr.x,
              hex[idx].point.y + off->curr.y, VCENTER | HCENTER);
    drawRegimentTitle(off->curr, idx);

    if (_dc->bClickOBJ)
    {
        idx = currentIndex;
        drawImage(counterTex->selectHex, hex[idx].point.x + off->curr.x,
                  hex[idx].point.y + off->curr.y, VCENTER | HCENTER);
        if(actRegimentAction() == false)
            drawRegimentTitle(off->curr, idx);
    }
}

void drawInfoCounters(float dt, bool subUI)//ProcUI_RegimentInfo.cpp
{
    if (_dc->focusRgIdx != -1 || _dc->currRgIdx != -1)
    {
        RegimentInfo* rg = regimentAtIndex(subUI ? _dc->currRgIdx : _dc->focusRgIdx);

        float offX = 210 + (subUI ? 170 : 0);
        float offY = 18;
        float sizeRate = 0.7f;
        for (int i = 0; i < rg->count; i++)
        {
            ArmyCounter* unit = rg->army[i];
            float position = 152 * i;

            if ( unit->type == 99 )
                continue;
            Texture* type = counterTex->w[unit->type];
            drawImage(type, offX, offY + position, TOP | LEFT,
                0, 0, type->width, type->height, sizeRate, sizeRate, 2, 0);

            Texture* nation = counterTex->sh[unit->nation];
            drawImage(nation, offX + 4, offY + position + 6, TOP | LEFT,
                0, 0, nation->width, nation->height, sizeRate, sizeRate, 2, 0);

            Texture* pattern = counterTex->bp[unit->nation][unit->pattern];
            drawImage(pattern, offX + 48, offY + position + 56, TOP | LEFT,
                0, 0, pattern->width, pattern->height, sizeRate, sizeRate, 2, 0);

            Texture* unitId = regimentText->counterID[rg->army[i]->idx];
            drawImage(unitId, offX + 40, offY + position + 9, TOP | LEFT,
                0, 0, unitId->width, unitId->height, 0.9, 0.9, 2, 0);

            int aa, dd, space = 0;
            unit->aup == 0 ? aa = unit->ap : aa = unit->aup;
            unit->dup == 0 ? dd = unit->dp : dd = unit->dup;
            aa < 10 && dd < 10 ? space = 12 : space = 0; // 한자리수 간격 좁히기
            if (unit->aup) aa += 51;  // 보정시
            if (unit->dup) dd += 51;  // 녹색

            Texture** power = counterTex->tWnum;
            drawImage(power[aa], offX + space + 45, offY + position + 23, TOP | LEFT,
                0, 0, power[aa]->width, power[dd]->height, sizeRate, sizeRate, 2, 0);
            drawImage(power[dd], offX - space + 104, offY + position + 23, TOP | LEFT,
                0, 0, power[aa]->width, power[dd]->height, sizeRate, sizeRate, 2, 0);
        }
    }
    resetRGBA();
}

void drawStepCounters(float dt)//ProcUI_RegimentInfo.cpp
{
    if ( _dc->focusRgIdx != -1 )
    {
        RegimentInfo* rg = regimentAtIndex( _dc->focusRgIdx );

        float offX, offY = 3;
        float sizeRate = 0.7f;
        for (int i = 0; i < rg->count; i++)
        {
            ArmyCounter* unit = rg->army[i];
            float position = 152 * i;

            int typeImgNum, strArrIdx;
            switch (unit->stepNum)//step에 따른 사용 이미지/공방 결정
            {
                case 3:
                    unit->nation < 5 ? typeImgNum = 2 : typeImgNum = 8;
                    strArrIdx = 4;
                    break;
                case 2:
                    unit->nation < 5 ? typeImgNum = 4 : typeImgNum = 10;
                    strArrIdx = 2;
                    break;
                case 1:
                    unit->nation < 5 ? typeImgNum = 5 : typeImgNum = 11;
                    strArrIdx = 0;
                    break;
            }

            for (int j = 0; j < unit->stepNum; j++)
            {
                offX = 50 + (210 * (2-j));

                if ( j != unit->stepCurr - 1 )  setRGBA(70, 70, 70, 255);

                Texture* type = counterTex->w[typeImgNum - j];
                drawImage(type, offX, offY + position, TOP | LEFT,
                          0, 0, type->width, type->height, sizeRate, sizeRate, 2, 0);

                Texture* nation = counterTex->sh[unit->nation];
                drawImage(nation, offX + 4, offY + position + 6, TOP | LEFT,
                          0, 0, nation->width, nation->height, sizeRate, sizeRate, 2, 0);

                Texture* pattern = counterTex->bp[unit->nation][unit->pattern];
                drawImage(pattern, offX + 48, offY + position + 56, TOP | LEFT,
                          0, 0, pattern->width, pattern->height, sizeRate, sizeRate, 2, 0);

                Texture* unitId = regimentText->counterID[rg->army[i]->idx];
                drawImage(unitId, offX + 40, offY + 9 + position, TOP | LEFT,
                          0, 0, unitId->width, unitId->height, 0.9, 0.9, 2, 0);

                int aa = unit->strength[strArrIdx - (j*2)];
                int dd = unit->strength[strArrIdx - (j*2) + 1];
                int space = 0;
                aa < 10 && dd < 10 ? space = 12 : space = 0; // 한자리수 간격 좁히기

                Texture** power = counterTex->tWnum;
                drawImage(power[aa], offX + space + 45, offY + position + 23, TOP | LEFT,
                          0, 0, power[aa]->width, power[dd]->height, sizeRate, sizeRate, 2, 0);
                drawImage(power[dd], offX - space + 104, offY + position + 23, TOP | LEFT,
                          0, 0, power[aa]->width, power[dd]->height, sizeRate, sizeRate, 2, 0);

                resetRGBA();
                if ( unit->stepMod && (j == unit->stepNum - 1) ) // downsize, x mark
                {
                    Texture* t = counterTex->mark[3];
                    drawImage(t,offX + (type->width * sizeRate / 2),
                                offY + position + (type->height * sizeRate / 2),
                                VCENTER | HCENTER, 0, 0, t->width, t->height,
                                0.5, 0.5,2, 0);
                }
            }
        }//for (int i = 0; i < rg->count; i++)
    }//if ( _dc->focusRgIdx != -1 )
}

void drawWarCounters(float dt) // RegimentAction.cpp
{
    int i, j;
    float offX, offY = 18;
    float sizeRate = 0.7f;

    for (i = 0; i < 2; i++)
    {
        RegimentInfo* rg = _dc->rg[i];

        offX = (i == 0 ? 100 : _dc->battleW - 100 - 150); // 배틀 UI 끝 - 영웅 체력 - 자기 이미지 w

        for (j = 0; j < rg->count; j++)
        {
            ArmyCounter* unit = rg->army[j];
            float position = 152 * j;

            if ( unit->type == 99 )
                continue;
            Texture* type = counterTex->w[unit->type];
            drawImage(type, offX, offY + position, TOP | LEFT,
                      0, 0, type->width, type->height, sizeRate, sizeRate, 2, 0);

            Texture* nation = counterTex->sh[unit->nation];
            drawImage(nation, offX + 4, offY + position + 6, TOP | LEFT,
                      0, 0, nation->width, nation->height, sizeRate, sizeRate, 2, 0);

            Texture* pattern = counterTex->bp[unit->nation][unit->pattern];
            drawImage(pattern, offX + 48, offY + position + 56, TOP | LEFT,
                      0, 0, pattern->width, pattern->height, sizeRate, sizeRate, 2, 0);

            Texture* unitId = regimentText->counterID[rg->army[i]->idx];
            drawImage(unitId, offX + 40, offY + position + 9, TOP | LEFT,
                      0, 0, unitId->width, unitId->height, 0.9, 0.9, 2, 0);

            int aa, dd, space = 0;
            unit->aup == 0 ? aa = unit->ap : aa = unit->aup;
            unit->dup == 0 ? dd = unit->dp : dd = unit->dup;
            aa < 10 && dd < 10 ? space = 8 : space = 0; // 한자리수 간격 좁히기
            if (unit->aup) aa += 51;  // 보정시
            if (unit->dup) dd += 51;  // 녹색

            Texture** power = counterTex->tWnum;
            drawImage(power[aa], offX + space + 45, offY + position + 23, TOP | LEFT,
                      0, 0, power[aa]->width, power[dd]->height, sizeRate, sizeRate, 2, 0);
            drawImage(power[dd], offX - space + 104, offY + position + 23, TOP | LEFT,
                      0, 0, power[aa]->width, power[dd]->height, sizeRate, sizeRate, 2, 0);
        }
    }

    resetRGBA();
}

