#include "ProcUI_Commander.h"

#include "iStd.h"
#include "LoadMap.h"
#include "DrawCounters.h"
#include "Proc.h"
#include "ProcUI_MapKey.h"
#include "ProcUI_MiniMap.h"
#include "ProcUI_RegimentInfo.h"
#include "RegimentAction.h"

static Commander* cm;

void drawComBorder(float dt);
void drawGeneralName(iPopup* pop, float dt, float rate);

#define POPMAX 6
void createCommander()
{
	cm = new Commander;
	cm->popCom = new iPopup*[POPMAX];
	cm->imgBtn = new iImage*[POPMAX - 1];

	Texture* tex;
	int i, j;
	iPopup* pop = new iPopup(1);
	iImage* img = new iImage();
	iGraphics* g = iGraphics::share();
	cm->comRt = iSizeMake(240, 360);
	cm->btnPos = iPointMake(devSize.width - cm->comRt.width, 131);

	g->init(cm->comRt);
	setRGBA(103, 153, 255, 150);
	g->fillRect(5, 5, cm->comRt.width - 10, cm->comRt.height - 10, 20);

	tex = g->getTexture();

	img->addObject(tex);
	freeImage(tex);
	pop->addObject(img);

	pop->style = iPopupAlpha;
	pop->openPoint =
    pop->closePoint = iPointZero;
	pop->methodFigure[0] = drawComBorder;
	pop->selected = 0;
	cm->popCom[0] = pop;

	const char* str[6] =
	{
		"assets/menu/btn_arrowframe%d.png",
		"assets/menu/btn_red%d.png",

		"FOCUS",
		"INFO",
		"MOVE",
		"NONE",
	};

	igImage* igs[6];
	for (i = 0; i < 4; i++)
		igs[i] = g->createIgImage(str[i / 2], i % 2);//0011 0101

	pop = new iPopup();
	img = new iImage();
	resetRGBA();
	setStringName("assets/font/RixYeoljeongdo_Regular.ttf");
	setStringSize(35);
	setStringRGBA(229, 216, 92, 255);
	setStringBorder(0);

	iSize size = iSizeMake(207, 77);
	for (i = 0; i < 2; i++)
	{
		g->init(size);
		g->drawIgImage(igs[i], 0, 0, TOP | LEFT);
		g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, str[2]);

		tex = g->getTexture();
		img->addObject(tex);
		img->position = iPointMake(17, 40);
		freeImage(tex);
	}
	pop->addObject(img);

	pop->style = iPopupAlpha;
	pop->openPoint =
	pop->closePoint = iPointZero;
	cm->imgBtn[0] = img;
	cm->popCom[1] = pop;

	size = iSizeMake(170, 75);
	for (i = 0; i < 3; i++)
	{
		pop = new iPopup();
		img = new iImage();

		for (j = 0; j < 2; j++)
		{
			g->init(size);
			g->drawIgImage(igs[j + 2], 0, 0, TOP | LEFT);
			g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, str[i + 3]);

			tex = g->getTexture();
			img->addObject(tex);
			img->position = iPointMake(34, 120 + i * 76);
			freeImage(tex);
		}
		pop->addObject(img);

		pop->style = iPopupAlpha;
		pop->openPoint =
		pop->closePoint = iPointZero;
		cm->imgBtn[i + 1] = img;
		cm->popCom[i + 2] = pop;
	}

	pop = new iPopup();
	img = new iImage();
	for (i = 0; i < 2; i++)
        igs[i + 4] = g->createIgImage("assets/map/army_title_round%d.png", i);

	size = iSizeMake(240, 50);

	for (i = 0; i < 2; i++)
    {
        g->init(size);
        g->drawIgImage(igs[i + 4], 0, 0, TOP | LEFT,
					   0, 0, 0, 0, 1.27, 0.7, 2, 0);

        tex = g->getTexture();
        img->addObject(tex);
		img->position = iPointMake(0, - 10);
        freeImage(tex);
    }
	pop->addObject(img);

	pop->style = iPopupAlpha;
	pop->openPoint =
	pop->closePoint = iPointZero;
	pop->methodTogether = drawGeneralName;
	cm->imgBtn[4] = img;
	cm->popCom[5] = pop;

	cm->offPop = iPointMake(cm->btnPos.x, cm->btnPos.y);
	cm->bMove = cm->bClick = false;
	cm->bFocus = cm->bInfoBtn = cm->bMoveBtn = cm->bSkillBtn = false;

	for (i = 0; i < 6; i++)
	    g->freeIgImage(igs[i]);
}

void freeCommander()
{
	for (int i = 0; i < POPMAX; i++)
		delete cm->popCom[i];
	delete cm->popCom;

	delete cm->imgBtn;

	delete cm;
}

void drawComBorder(float dt)
{
	uint8 ca = (uint8)(255 * cm->popCom[0]->alpha);
	setRGBA(218, 165, 32, ca > 200 ? 200 : ca);
	setLineWidth(10);
	drawRect(0, 0, cm->comRt.width, cm->comRt.height, 20);
	setLineWidth(1);
	resetRGBA();
}

void drawGeneralName(iPopup* pop, float dt, float rate)
{
    bool ally;
    Texture* t;
    cm->bFocus ? ally = cm->saveAlly : ally = _dc->ally;
    cm->bFocus ? t = cm->saveName : t = _dc->heroName;

    cm->imgBtn[4]->setTexObject(ally);
    drawImage(t, cm->comRt.width / 2, 25, VCENTER | HCENTER,
              0, 0, t->width, t->height, 2, 2, 2, 0);
};

void drawCommander(float dt)
{
	resetRGBA();
	cm->bFocus ? cm->imgBtn[0]->setTexObject(1) : cm->imgBtn[0]->setTexObject(0);
	cm->bInfoBtn ? cm->imgBtn[1]->setTexObject(1) : cm->imgBtn[1]->setTexObject(0);
	cm->bMoveBtn ? cm->imgBtn[2]->setTexObject(1) : cm->imgBtn[2]->setTexObject(0);
	cm->bSkillBtn ? cm->imgBtn[3]->setTexObject(1) : cm->imgBtn[3]->setTexObject(0);

    for (int i = 0; i < POPMAX; i++)
    {
        iPopup* pop = cm->popCom[i];
		if ( pop->selected > 0 )
			cm->imgBtn[pop->selected - 1]->setTexObject(1);
        iPoint o = pop->openPoint;
        iPoint c = pop->closePoint;
        pop->openPoint += cm->offPop;
        pop->closePoint += cm->offPop;
        pop->paint(dt);
        pop->openPoint = o;
        pop->closePoint = c;
    }
}

void showCommander(bool show)
{
	for (int i = 0; i < POPMAX; i++)
		cm->popCom[i]->show(show);

	if (show == false)
	{
		cm->bInfoBtn = false;
		showPopRegimentInfo(false);
		cm->bMoveBtn = false;
		showRegimentAction(false);
		cm->bSkillBtn = false;
	}
}

bool focusCommander()
{
	return cm->bFocus;
}

bool keyCommander(iKeyStat stat, iPoint point)
{
	if (movingMapkey() ||
		movingMinimap())
		return false;

	int currIdx = currentIndex, i, j = 0;
	iRect rt = iRectMake(cm->offPop.x, cm->offPop.y, cm->comRt.width, cm->comRt.height + 10);
	iPopup* pop = cm->popCom[0];
	switch (stat)
	{
	case iKeyStatBegan:
	{
		if (pop->bShow && containPoint(point, rt))
		{
			drawOrder(2);

			for (i = 0; i < 4; i++)
			{
				if (containPoint(point, cm->imgBtn[i]->touchRect(cm->offPop)))
				{
					j = i + 1;
					cm->bClick = true;
					if (pop->selected != j)
						pop->selected = j;
					break;
				}
			}

			if (cm->bClick == false)
			{
				cm->bMove = true;
				cm->offPrev = point;
			}
		}
		break;
	}

	case iKeyStatMoved:
	{
		if (cm->bMove)
		{
			cm->offPop += point - cm->offPrev;
			cm->offPrev = point;
			restrictMap(cm->offPop, iPointZero, iPointMake(devSize.width, devSize.height),
               cm->comRt.width, cm->imgBtn[4]->tex->height);
		}

		if (cm->bClick)
		{
			i = pop->selected;
			if (i==0) break;
		}
		break;
	}

	case iKeyStatEnded:
	{
		cm->bMove = false;
		if (pop->selected == 0)
			break;
		bool ally;
		cm->bFocus ? ally = cm->saveAlly : ally = _dc->ally;

		if (pop->selected == 1)//focus
		{
			if (cm->bMoveBtn == false)//move와 중복 실행 안됨
			{
				cm->bFocus = !cm->bFocus;

				if (cm->bFocus)
				{
					_dc->focusIdx = currIdx;
					_dc->bClickOBJ = true;
					cm->saveAlly = _dc->ally;
					cm->saveName = _dc->heroName;
				}
				else
				{
					cm->bInfoBtn = false;
					showPopRegimentInfo(false);
					_dc->bClickOBJ = false;
					currIdx = _dc->focusIdx;
				}
			}
		}
		else if (pop->selected == 2)//info
		{
			if (cm->bMoveBtn == false)//move와 중복 실행 안됨
			{
				cm->bInfoBtn = !cm->bInfoBtn;
				cm->bInfoBtn ? showPopRegimentInfo(true) : showPopRegimentInfo(false);
			}
		}
		else if (pop->selected == 3 && ally)//move
		{
			if (cm->bMoveBtn)
			{
				cm->bMoveBtn = false;
				showRegimentAction(false);
				currIdx = _dc->focusIdx;
				_dc->bClickOBJ = false;
			}
			else
			{
				int armyIdx = getRegimentIndex(currIdx);
				if (armyIdx != -1)
				{
					cm->bMoveBtn = true;

					if (_dc->focusIdx == -1)
						_dc->focusIdx = currIdx;
					cm->bFocus = false;
					cm->bInfoBtn = false;
					cm->bSkillBtn = false;
					showPopRegimentInfo(false);
					_dc->bClickOBJ = true;
					actionRangeTracking();

					showRegimentAction(true);
				}
			}
		}
		else if (pop->selected == 4 && ally)//skill
		{
			if (cm->bMoveBtn == false)//move와 중복 실행 안됨
			{
				cm->bSkillBtn = !cm->bSkillBtn;
			}
		}

		if (cm->bClick)
		{
			cm->bClick = false;
			pop->selected = 0;
		}
		break;
	}
	}

	if (pop->bShow && containPoint(point, rt)) // currentIndex 유지
	{
		currentIndex = currIdx;
		return true;
	}

	return false;
}

bool movingCommander()
{
	return cm->bMove || cm->bClick;
}

void cmBmoveBtnOff()
{
    cm->bMoveBtn = false;
    showRegimentAction(false);
    _dc->bClickOBJ = false;
}

