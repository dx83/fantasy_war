#include "ProcUI_RegimentInfo.h"

#include "iStd.h"

#include "LoadMap.h"
#include "RegimentMgr.h"
#include "DrawCounters.h"

#include "ProcUI_MiniMap.h"
#include "ProcUI_MapKey.h"

RegimentInfoTex* rgTexture;
PopRegimentInfo* popRgInfo;
void drawUIBox(float dt);
void drawTouch(float dt);
void drawStep(float dt);

#define UITEXTMAX 17
void createPopRegimentInfo()
{
    int i;
    rgTexture = new RegimentInfoTex;

    const char* uiText[UITEXTMAX] =
    {
        "지휘관", "장군", "국왕",
        "국가 :", "영웅 :", "종족 :", "신분 :", "-",
        "소속 부대", "직책 :", "STEPs",
        "통솔력", "마법력",
        //"편성 패턴", "특수 능력",
        "군단", "전투력",
        "총공격력", "총방어력",
    };
    const char* charText[17] =
    {
        "0", "1", "2", "3", "4", "5",
        "S", "A", "B", "C", "D",//83, 65, 66, 67, 68
        "A", "B", "C", "0", "1", "2"
    };

    rgTexture->entry = new Texture*[UITEXTMAX];
    rgTexture->status = new Texture*[17];
    rgTexture->steps = new Texture*[35];
    rgTexture->roundBox = new Texture*[4];
    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(80, 25);
    // entry text
    setStringName("assets/font/Maplestory_Bold.ttf");
    setStringSize(20);
    setStringRGBA(50, 50, 50, 255);
    setStringBorder(2);
    setStringBorderRGBA(153, 112, 0, 255);
    for (i = 0; i < UITEXTMAX; i++)
    {
        if (i == 15)
        {
            setStringSize(15);
            setStringRGBA(255, 20, 20, 255);
            setStringBorderRGBA(150, 150, 150, 255);
        }
        else if (i == 16)
            setStringRGBA(0, 0, 240, 255);

        g->init(size);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, uiText[i]);
        rgTexture->entry[i] = g->getTexture();
    }
    // status text
    setStringName("assets/font/GmarketSansTTFBold.ttf");
    setStringSize(40);
    setStringBorder(0);
    setLineWidth(5);
    size = iSizeMake(40, 40);
    for (i = 0; i < 17; i++)
    {
        switch (i)
        {
            case 0:	case 1:	case 10: case 13: case 14:
                setStringRGBA(53, 53, 53, 255); setRGBA(53, 53, 53, 255); break;
            case 2:	case 9:
                setStringRGBA(18, 102, 255, 255); setRGBA(18, 102, 255, 255); break;
            case 3:	case 8:	case 12: case 15:
                setStringRGBA(0, 255, 0, 255); setRGBA(0, 255, 0, 255); break;
            case 4:	case 7:
                setStringRGBA(255, 36, 36, 255); setRGBA(255, 36, 36, 255); break;
            case 5:	case 6:	case 11: case 16:
                setStringRGBA(255, 228, 0, 255); setRGBA(255, 228, 0, 255); break;
        }
        g->init(size);
        g->drawRect(0, 0, size.width, size.height, 10);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%s", charText[i]);
        rgTexture->status[i] = g->getTexture();
    }
    setLineWidth(1);
    // steps
    setStringSize(50);
    setStringRGBA(20, 20, 20, 255);
    size = iSizeMake(50, 50);
    g->init(size);
    g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "/");
    rgTexture->steps[34] = g->getTexture();

    size = iSizeMake(25, 25);
    setStringSize(15);
    setStringRGBA(0, 255, 0, 255);// green
    for (i = 0; i < 17; i++)	//small size number
    {
        g->init(size);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%d", i);
        rgTexture->steps[i] = g->getTexture();
    }
    size = iSizeMake(40, 40);
    setStringSize(30);
    setStringRGBA(255, 255, 255, 255);
    for (i = 0; i < 17; i++)	//big size number
    {
        g->init(size);
        g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%d", i);
        rgTexture->steps[i + 17] = g->getTexture();
    }

    // step box Main
    size = iSizeMake(189, 36);//220
    g->init(size);
    setRGBA(36, 123, 138, 220);
    g->fillRect(0, 0, size.width, size.height, 10);
    g->drawString(0, 0, TOP | LEFT, "");
    rgTexture->roundBox[0] = g->getTexture();
    // step box Sub
    size = iSizeMake(90, 85);
    g->init(size);
    g->fillRect(0, 0, size.width, size.height, 10);
    g->drawString(0, 0, TOP | LEFT, "");
    rgTexture->roundBox[1] = g->getTexture();
    // combat value Main
    size = iSizeMake(179, 70);
    g->init(size);
    setRGBA(61, 183, 204, 150);
    g->fillRect(0, 0, size.width, size.height, 10);
    g->drawString(0, 0, TOP | LEFT, "");
    rgTexture->roundBox[2] = g->getTexture();
    // combat value Sub
    size = iSizeMake(90, 174);
    g->init(size);
    setRGBA(61, 183, 204, 150);
    g->fillRect(0, 0, size.width, size.height, 10);
    g->drawString(0, 0, TOP | LEFT, "");
    rgTexture->roundBox[3] = g->getTexture();
#if 0 // not use
    // pattern box
	size = iSizeMake(220, 217);
	g->init(size);
	setRGBA(61, 183, 204, 150);
	g->fillRect(0, 0, size.width, size.height, 10);
	g->drawString(0, 0, TOP | LEFT, "");
	rgTexture->roundBox[2] = g->getTexture();
	// special ability box
	size = iSizeMake(220, 163);
	g->init(size);
	setRGBA(61, 183, 204, 150);
	g->fillRect(0, 0, size.width, size.height, 10);
	g->drawString(0, 0, TOP | LEFT, "");
	rgTexture->roundBox[3] = g->getTexture();
#endif
	// Steps Counters
    popRgInfo = new PopRegimentInfo;
    popRgInfo->pop = new iPopup*[2];

    iPopup* pop = new iPopup(2);
    pop->style = iPopupMove;
    pop->openPoint = iPointMake(-600, 0);//-425
    pop->closePoint = iPointMake(0, 0);
    pop->methodFigure[0] = drawUIBox;
    pop->methodFigure[1] = drawTouch;
    popRgInfo->pop[0] = pop;

    pop = new iPopup(1);
    pop->style = iPopupNone;
    pop->openPoint =
    pop->closePoint = iPointMake(375, 15);
    pop->methodFigure[0] = drawStep;
    popRgInfo->pop[1] = pop;

    setStringName("assets/font/GmarketSansTTFBold.ttf");
    setStringSize(40);
    setStringRGBA(51, 96, 96, 255);
    setStringBorder(0);
    setLineWidth(5);
    size = iSizeMake(40, 260);

    popRgInfo->steps = new Texture*[3];
    float w = size.width / 2;
    for ( i = 0; i < 3; i++ )
    {
        int modify = i < 2 ? 0 : 20;
        g->init(size);
        g->drawString(w, 20 + modify, VCENTER | HCENTER, "%d", 3 - i);
        g->drawString(w, 60 + modify, VCENTER | HCENTER, "S");
        g->drawString(w, 100 + modify, VCENTER | HCENTER, "T");
        g->drawString(w, 140 + modify, VCENTER | HCENTER, "E");
        g->drawString(w, 180 + modify, VCENTER | HCENTER, "P");
        if ( i < 2)
            g->drawString(w, 220, VCENTER | HCENTER, "S");
        popRgInfo->steps[i] = g->getTexture();
    }

    popRgInfo->bTouch = false;
    popRgInfo->bStep = false;
}

void freePopRegimentInfo()
{
	int i;
	for (i = 0; i < UITEXTMAX; i++)
		freeImage(rgTexture->entry[i]);
	delete rgTexture->entry;
	for (i = 0; i < 17; i++)
		freeImage(rgTexture->status[i]);
	delete rgTexture->status;
	for (i = 0; i < 35; i++)
		freeImage(rgTexture->steps[i]);
	delete rgTexture->steps;
	for (i = 0; i < 4; i++)
		freeImage(rgTexture->roundBox[i]);
	delete rgTexture->roundBox;

	delete rgTexture;

	for (i = 0; i < 2; i++)
	    delete popRgInfo->pop[i];
	delete popRgInfo->pop;

	for (i = 0; i < 3; i++)
	    freeImage(popRgInfo->steps[i]);
	delete popRgInfo->steps;

	delete popRgInfo;
}

void drawUIBox(float dt)
{
	int i, idx;

	_dc->focusIdx == -1 ? idx = currentIndex : idx = _dc->focusIdx;

    int armyIdx = getRegimentIndex(idx);
	if (armyIdx == -1)
	    showPopRegimentInfo(false);
	else
        _dc->focusRgIdx = armyIdx;

	if (_dc->focusRgIdx != -1)
	{
        // MAIN INFO===============================================
        setRGBA(51, 76, 76, 128);
        fillRect(5, 10, 365, 620);		//main box

        setRGBA(200, 200, 200, 200);
        fillRect(205, 15, 160, 611);	    //counter box

        resetRGBA();
        drawInfoCounters(dt, false);

        setRGBA(200, 200, 200, 200);
        for (i = 0; i < 5; i++)
            fillRect(15, 225 + i * 33, 179, 25);// entry box
        //fillRect(35, 245, 179, 25);//appoint
        // status
        fillRect(15, 390, 85, 25);
        fillRect(109, 390, 85, 25);
        fillRect(15, 423, 85, 45);
        fillRect(109, 423, 85, 45);
        // combat
        fillRect(15, 520, 110, 25);
        // pattern
        //fillRect(35, 408, 100, 25);
        // special ablility
        //fillRect(35, 666, 100, 25);

        resetRGBA();
        RegimentInfo* rg = regimentAtIndex(_dc->focusRgIdx);
        Texture** entry = rgTexture->entry;
        //
        Texture* heraldry = counterTex->bh[rg->gi->nation];
        drawImage(heraldry, 5, 18, TOP | LEFT);
        //
        drawImage(entry[3], 5, 225, TOP | LEFT);
        Texture* nation = regimentText->nation[rg->gi->nation];
        drawImage(nation, 75, 225, TOP | LEFT);
        //
        drawImage(entry[4], 5, 258, TOP | LEFT);
        Texture* heroName = regimentText->heroName[rg->gi->idx];
        drawImage(heroName, 75, 258, TOP | LEFT);
        //
        drawImage(entry[5], 5, 292, TOP | LEFT);
        Texture* heroRace = regimentText->heroRace[rg->gi->nation][rg->gi->race];
        drawImage(heroRace, 75, 292, TOP | LEFT);
        //
        Texture* mark = counterTex->mark[rg->gi->mark];
        drawImage(mark, 75, 336, VCENTER | HCENTER,
                  0, 0, mark->width, mark->height, 0.8f, 0.8f, 2, 0);
        drawImage(entry[rg->gi->mark], 105, 325, TOP | LEFT);
        //
        drawImage(entry[6], 2, 358, TOP | LEFT);
        if (rg->gi->note[0])
        {
            Texture* heroNote = regimentText->heroNote[rg->gi->idx];
            drawImage(heroNote, 75, 358, TOP | LEFT);
        }
        else
            drawImage(entry[7], 88, 358, TOP | LEFT);
        //regiment list
        //drawImage(entry[8], 295, 15, TOP | LEFT);
        //appoint
        //drawImage(entry[9], 35, 245, TOP | LEFT);
        //if (rg->gi->appoint)
        //{
        //	// 직책
        //}
        //else
        //	drawImage(entry[7], 130, 245, TOP | LEFT);
        //

        drawImage(entry[11], 17, 390, TOP | LEFT);
        drawImage(entry[12], 111, 390, TOP | LEFT);

        Texture** status = rgTexture->status;
        drawImage(status[rg->gi->leaderRank - 54], 17, 425, TOP | LEFT);
        drawImage(status[rg->gi->leaderNum + 14], 58, 425, TOP | LEFT);
        int idx = rg->gi->magicRank == 83 ? 6 : rg->gi->magicRank - 58;;
        drawImage(status[idx], 111, 425, TOP | LEFT);
        drawImage(status[rg->gi->magicNum], 152, 425, TOP | LEFT);
        //
        Texture* steps = rgTexture->steps[rg->gi->currStep];
        drawImage(rgTexture->roundBox[0], 10, 476, TOP | LEFT);
        drawImage(steps, 22, 502, VCENTER | HCENTER);
        setLineWidth(7);
        setRGBA(219, 0, 0, 255);
        for (i = 0; i < rg->gi->maxSteps; i++)
        {
            if (i > rg->gi->currStep - 1)
                setRGBA(50, 50, 50, 255);
            drawLine(30 + i * 12, 482, 40 + i * 12, 506);
        }
        setLineWidth(1);
        // combat
        resetRGBA();
        drawImage(entry[13], 0, 520, TOP | LEFT);
        drawImage(entry[14], 52, 520, TOP | LEFT);
        drawImage(rgTexture->roundBox[2], 15, 553, TOP | LEFT);

        drawImage(mk->tDef[3], 25, 560, TOP | LEFT);

        Hexagon* hexs = getHexInfo();
        Hexagon* hCurr = &hexs[_dc->focusIdx == -1 ? currentIndex : _dc->focusIdx];

        drawImage(mk->tDef[hCurr->defBonus], 155, 560, TOP | LEFT);

        Texture** power = counterTex->tWnum;
        drawImage(entry[15], 7, 593, TOP | LEFT);
        drawImage(entry[16], 95, 593, TOP | LEFT);
        drawImage(power[rg->ar], 70, 590, TOP | LEFT,
                  0, 0, power[rg->ar]->width, power[rg->ar]->height, 0.6, 0.6, 2, 0);
        drawImage(power[rg->dr], 158, 590, TOP | LEFT,
                  0, 0, power[rg->dr]->width, power[rg->dr]->height, 0.6, 0.6, 2, 0);
        //drawImage(entry[13], 45, 408, TOP | LEFT);
        //drawImage(rgTexture->roundBox[2], 30, 441, TOP | LEFT);
        //for (i = 0; i < 8; i++)
        //{
        //	int firstIdx = rg->gi->patterns[i][0];
        //	int secondIdx = rg->gi->patterns[i][1];
        //	if (i > 0 && firstIdx == 0 && secondIdx == 0)	break;
        //
        //	Texture* patterns = counterTex->bp[firstIdx][secondIdx];
        //	drawImage(patterns, 34 + (i % 3) * 72, 448 + (i / 3) * 70, TOP | LEFT,
        //		0, 0, patterns->width, patterns->height, 0.5f, 0.5f, 2, 0);
        //}
        //
        //drawImage(entry[14], 45, 666, TOP | LEFT);
        //drawImage(rgTexture->roundBox[3], 30, 699, TOP | LEFT);
        // MAIN INFO===============================================

        // SUB INFO================================================
        if ( popRgInfo->bStep || _dc->bClickOBJ == false ) return; // step reference || SUB UI

        armyIdx = getRegimentIndex(currentIndex);
        _dc->currRgIdx = armyIdx;

        if ( armyIdx == -1 || armyIdx == _dc->focusRgIdx ) return; // vacant || oneself

        setRGBA(51, 76, 76, 128);
        fillRect(370, 10, 266, 620);	//sub box

        setRGBA(200, 200, 200, 200);
        fillRect(375, 15, 160, 611);	//counter box

        //fillRect(535, 43, 65, 25);	// steps
        fillRect(539, 126, 92, 25);	// hero name
        // status
        fillRect(546, 159, 77, 25);
        fillRect(546, 192, 77, 30);
        fillRect(546, 230, 77, 25);
        fillRect(546, 263, 77, 30);
        // combat
        //fillRect(546, 394, 77, 50);
        fillRect(556, 394, 57, 25);
        fillRect(546, 419, 77, 25);

        resetRGBA();
        rg = regimentAtIndex(armyIdx);

        Texture* heraldSub = counterTex->bh[rg->gi->nation];
        drawImage(heraldSub, 535, 18, TOP | LEFT,
                  0, 0, heraldSub->width, heraldSub->height, 0.5f, 0.5f, 2, 0);
        //
        heroName = regimentText->heroNameTitle[rg->gi->idx];
        drawImage(heroName, 584, 126, TOP | HCENTER);
        //
        drawImage(entry[11], 544, 159, TOP | LEFT);
        drawImage(entry[12], 544, 230, TOP | LEFT);
        //
        Texture* statSub = rgTexture->status[rg->gi->leaderRank - 54];
        drawImage(statSub, 554, 193, TOP | LEFT,
                  0, 0, statSub->width, statSub->height, 0.7f, 0.7f, 2, 0);

        statSub = rgTexture->status[rg->gi->leaderNum + 14];
        drawImage(statSub, 588, 193, TOP | LEFT,
                  0, 0, statSub->width, statSub->height, 0.7f, 0.7f, 2, 0);

        rg->gi->magicRank == 83 ? idx = 6 : idx = rg->gi->magicRank - 58;
        statSub = rgTexture->status[idx];
        drawImage(statSub, 554, 264, TOP | LEFT,
                  0, 0, statSub->width, statSub->height, 0.7f, 0.7f, 2, 0);

        statSub = rgTexture->status[rg->gi->magicNum];
        drawImage(statSub, 588, 264, TOP | LEFT,
                  0, 0, statSub->width, statSub->height, 0.7f, 0.7f, 2, 0);
        //
        //drawImage(entry[10], 535, 46, TOP | LEFT,
        //	0, 0, entry[10]->width, entry[10]->height, 0.8f, 0.8f, 2, 0);

        Texture** stepNum = rgTexture->steps;
        drawImage(rgTexture->roundBox[1], 540, 301, TOP | LEFT);
        drawImage(stepNum[34], 585, 345, VCENTER | HCENTER,
                  0, 0, stepNum[34]->width, stepNum[34]->height, 1, 1.5, 2, 0);


        if ( rg->gi->currStep < rg->gi->maxSteps )
            setRGBA(204, 61, 61, 255);
        else
            setRGBA(0, 255, 0, 255);
        drawImage(stepNum[rg->gi->currStep + 17], 565, 321, VCENTER | HCENTER);
        setRGBA(0, 255, 0, 255);
        drawImage(stepNum[rg->gi->maxSteps + 17], 607, 361, VCENTER | HCENTER);
        resetRGBA();

        // combat
        drawImage(entry[13], 546, 394, TOP | LEFT);
        drawImage(entry[14], 546, 419, TOP | LEFT);
        drawImage(rgTexture->roundBox[3], 540, 449, TOP | LEFT);//y 452

        drawImage(mk->tDef[3], 560, 455, TOP | LEFT,
                  55, 0, mk->tDef[3]->width, mk->tDef[3]->height, 1, 1, 2, 0);

        hCurr = &hexs[currentIndex];
        drawImage(mk->tDef[hCurr->defBonus], 572, 480, TOP | LEFT);

        drawImage(entry[15], 546, 510, TOP | LEFT);
        drawImage(entry[16], 546, 565, TOP | LEFT);
        drawImage(power[rg->ar], 563, 530, TOP | LEFT,
                  0, 0, power[rg->ar]->width, power[rg->ar]->height, 0.7, 0.7, 2, 0);
        drawImage(power[rg->dr], 563, 585, TOP | LEFT,
                  0, 0, power[rg->dr]->width, power[rg->dr]->height, 0.7, 0.7, 2, 0);

        resetRGBA();
        drawInfoCounters(dt, true);
	}
}

void drawTouch(float dt)
{
    if ( popRgInfo->bTouch )
        setRGBA(250, 250, 230, 200);
    else
        setAlpha(0);
    fillRect(205, 15, 160, 611);	    //counter Touch box

    resetRGBA();
}

void drawStep(float dt)
{
    setRGBA(200, 200, 200, 200);

    fillRect(0, 0, 40, 611);
    fillRect(45, 0, 160, 611);

    fillRect(210, 0, 40, 611);
    fillRect(255, 0, 160, 611);

    fillRect(420, 0, 40, 611);
    fillRect(465, 0, 160, 611);
    resetRGBA();

    Texture** t = popRgInfo->steps;
    drawImage(t[0], 0, 315, VCENTER | LEFT);
    drawImage(t[1], 210, 315, VCENTER | LEFT);
    drawImage(t[2], 420, 315, VCENTER | LEFT);

    drawStepCounters(dt);
}

void drawPopRegimentInfo(float dt)
{
    popRgInfo->pop[0]->paint(dt);
}

void showPopRegimentInfo(bool show)
{
    if ( show == false )
        showStepReference(false);
    popRgInfo->pop[0]->show(show);
}

void drawStepReference(float dt)
{
    popRgInfo->pop[1]->paint(dt);
}

void showStepReference(bool show)
{
    popRgInfo->pop[1]->show( popRgInfo->bStep = show );
}

bool keyPopRegimentInfo(iKeyStat stat, iPoint point)
{
    if ( popRgInfo->pop[0]->stat != iPopupProc ) // 메인창이 완전히 열려 있을 때만 동작
        return false;

    iRect rt = iRectMake(205, 15, 160, 611);
	switch (stat)
	{
	case iKeyStatBegan:
    {
        if ( containPoint(point, rt) )
            popRgInfo->bTouch = true;
        break;
    }

	case iKeyStatMoved:
    {
        if ( !containPoint(point, rt) )
            popRgInfo->bTouch = false;
		break;
    }

	case iKeyStatEnded:
    {
        if ( popRgInfo->bTouch )
            popRgInfo->bStep ? showStepReference(false) : showStepReference(true);
        popRgInfo->bTouch = false;
        break;
    }
	}

	return containPoint(point, rt);
}

