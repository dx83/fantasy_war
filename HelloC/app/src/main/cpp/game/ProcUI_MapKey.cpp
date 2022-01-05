#include "ProcUI_MapKey.h"

#include "iStd.h"

#include "TextGroup.h"
#include "LoadMap.h"
#include "Proc.h"
#include "ProcUI_MiniMap.h"
#include "ProcUI_Commander.h"

MapKey* mk;
iPopup* tileInfo;

void drawStrInfo(float dt);
void insertToHex();

void createMapKey()
{
	int i;
	// MapKey
	mk = new MapKey;
	mk->maxTr = sizeof(terrain) / sizeof(TileType);
	mk->maxLd = sizeof(landmark) / sizeof(TileType);

	// popup tileInfo
	mk->w = 275, mk->h = 87;
	iPopup* pop = new iPopup(1);
	pop->style = iPopupNone;
	pop->openPoint = iPointZero;
	pop->closePoint = iPointZero;
	pop->methodFigure[0] = drawStrInfo;
	tileInfo = pop;
	
	mk->offPop = iPointMake(devSize.width - mk->w - 5, devSize.height - mk->h - 230);
	mk->bMove = false;

	// Info Text
	mk->max = mk->maxTr + mk->maxLd;
	mk->tLand = new Texture*[mk->max];
	mk->tMove = new Texture*[6];
	mk->tDef = new Texture*[4];
	mk->goal = new Texture*[2];
	setStringName("바탕");
	setStringSize(20);
	setStringBorder(1);
	setStringStyle(0);

	iGraphics* g = iGraphics::share();
	iSize size = iSizeMake(300, 30);
	for (i = 0; i < mk->max; i++)
	{
		g->init(size);
		if (i < mk->maxTr)
		{
			setStringRGBA(255, 255, 255, 255);
			setStringBorderRGBA(255, 255, 255, 255);
			g->drawString(0, size.height / 2, VCENTER | LEFT, "지형 : %s", terrain[i].name);
			terrain[i].index = i;
		}
		else
		{
			setStringRGBA(255, 204, 0, 255);
			setStringBorderRGBA(255, 204, 0, 255);
			int idx = i - mk->maxTr;
			g->drawString(0, size.height / 2, VCENTER | LEFT, "지명 : %s", landmark[idx].name);
			landmark[idx].index = i;
		}

		mk->tLand[i] = g->getTexture();
	}
	
	for (i = 0; i < 6; i++)
	{
		setStringRGBA(255, 255, 255, 255);
		setStringBorderRGBA(255, 255, 255, 255);
		g->init(size);
		if (i == 5)
			g->drawString(0, size.height / 2, VCENTER | LEFT, "소비 이동력 :");
		else if (i)
			g->drawString(0, size.height / 2, VCENTER | LEFT, "%d", i);
		else //if ( i == 0 )
		{
			setStringRGBA(103, 0, 0, 255);
			setStringBorderRGBA(103, 0, 0, 255);
			g->drawString(0, size.height / 2, VCENTER | LEFT, "이동 불가");
		}
		mk->tMove[i] = g->getTexture();
	}

	for (i = 0; i < 4; i++)
	{
		setStringRGBA(255, 255, 255, 255);
		setStringBorderRGBA(255, 255, 255, 255);
		g->init(size);
		if (i == 3)
			g->drawString(0, size.height / 2, VCENTER | LEFT, "방어력 보정 :");
		else if (i == 0)
			g->drawString(0, size.height / 2, VCENTER | LEFT, "+%d", i);
		else
		{
			setStringRGBA(0, 255, 0, 255);
			setStringBorderRGBA(0, 255, 0, 255);
			g->drawString(0, size.height / 2, VCENTER | LEFT, "+%d", i);
		}
		mk->tDef[i] = g->getTexture();
	}
	resetRGBA();

	size = iSizeMake(mk->w, mk->h);
	setStringName("assets/font/Roboto-Regular.ttf");
	setStringSize(20);
	setStringBorder(1);
	g->init(size);
	g->drawString(size.width / 2 - 15, 3, TOP | HCENTER, "-- 게임 목표 --");
	g->drawString(15, 30, TOP | LEFT, "북서쪽에 있는");
	g->drawString(15, 57, TOP | LEFT, "샨고스 요새를 점령");
	mk->goal[0] = g->getTexture();

    g->init(size);
    g->drawString(size.width / 2 - 15, 3, TOP | HCENTER, "-- 목표 달성 --");
    g->drawString(0, 30, TOP | LEFT, "게임을 즐겨주셔서 감사합니다.");
    g->drawString(0, 57, TOP | LEFT, "특전 : 이동력 무한");
    mk->goal[1] = g->getTexture();

	insertToHex();
}

void freeMapKey()
{
	int i;

	for (i = 0; i < mk->max; i++)
		freeImage(mk->tLand[i]);
	for (i = 0; i < 6; i++)
		freeImage(mk->tMove[i]);
	for (i = 0; i < 4; i++)
		freeImage(mk->tDef[i]);
	for (i = 0; i < 2; i++)
	    freeImage(mk->goal[i]);
	delete mk->tLand;
	delete mk->tMove;
	delete mk->tDef;
	delete mk->goal;
	
	delete mk;

	delete tileInfo;
}

void drawStrInfo(float dt)
{
	setRGBA(0, 130, 153, 200);
	fillRect(1, 1, mk->w, mk->h);
	setRGBA(180, 180, 180, 255);
	drawRect(1, 1, mk->w, mk->h);
	resetRGBA();

	if ( currentIndex == -1 )
	{
		drawImage(mk->goal[statVictory()], 15, 3, TOP | LEFT);
		return;
	}

	Hexagon* hexs = getHexInfo();
	Hexagon* hCurr = &hexs[currentIndex];
	if ( fogStatus() || hCurr->exHex )
    {
        drawImage(mk->goal[statVictory()], 15, 3, TOP | LEFT);
        return;
    }

	int n = hCurr->landmark;
	if (n != -1)
	{
		drawImage(mk->tLand[n], 15, 3, TOP | LEFT);
		drawImage(mk->tMove[5], 15, 30, TOP | LEFT);
		drawImage(mk->tMove[1], 150, 30, TOP | LEFT);
		drawImage(mk->tDef[3], 15, 57, TOP | LEFT);
		drawImage(mk->tDef[hCurr->defBonus], 150, 57, TOP | LEFT);
	}
	else
	{
		n = hCurr->terrain;
		if (n != -1)
		{
			drawImage(mk->tLand[n], 15, 3, TOP | LEFT);
			drawImage(mk->tMove[5], 15, 30, TOP | LEFT);
			n = hCurr->move;
			drawImage(mk->tMove[n], 150, 30, TOP | LEFT);
		}
	}
}

void drawTileInfo(float dt)
{
    iPoint o = tileInfo->openPoint;
    iPoint c = tileInfo->closePoint;
	tileInfo->openPoint += mk->offPop;
	tileInfo->closePoint += mk->offPop;
	tileInfo->paint(dt);
    tileInfo->openPoint = o;//tileInfo->openPoint -= mk->offPop;
    tileInfo->closePoint = c;//tileInfo->closePoint -= mk->offPop;
}

void showTileInfo(bool show)
{
	tileInfo->show(show);
}

bool keyTileInfo(iKeyStat stat, iPoint point)
{
	if (movingMinimap() ||
		movingCommander())
		return false;

	int currIdx = currentIndex;
    iRect rt = iRectMake(mk->offPop.x, mk->offPop.y, mk->w + 2, mk->h + 2);
	switch (stat)
	{
	case iKeyStatBegan:
	{
		if (containPoint(point, rt))
		{
            drawOrder(0);
			mk->bMove = true;
			mk->offPrev = point;
		}
		break;
	}

	case iKeyStatMoved:
	{
		if (mk->bMove)
		{
			mk->offPop += point - mk->offPrev;
			mk->offPrev = point;
			restrictMap(mk->offPop, iPointZero,
				iPointMake(devSize.width, devSize.height), mk->w + 2, mk->h + 2);
		}
		break;
	}

	case iKeyStatEnded:
		mk->bMove = false;
		break;
	}

    if (containPoint(point, rt)) // currentIndex 유지
    {
        currentIndex = currIdx;
        return true;
    }

	return false;
}

void insertToHex()
{
	Hexagon* hs = mapKeyInHex();
	
	int max = sizeof(tileTerrain) / sizeof(int) / 2;

	for (int i = 0; i < max; i++)
	{
		for (int j = 0; j < mk->maxTr; j++)
		{
			if (tileTerrain[i][1] == terrain[j].hexIdx)
			{
				int idx = tileTerrain[i][0];
				hs[idx].move = terrain[j].move;
				hs[idx].terrain = terrain[j].index;
				break;
			}
		}
	}

	for (int i = 0; i < mk->maxLd; i++)
	{
		hs[landmark[i].hexIdx].landmark = landmark[i].index;
		hs[landmark[i].hexIdx].defBonus = landmark[i].defBonus;
	}
}

bool movingMapkey()
{
	return mk->bMove;
}

