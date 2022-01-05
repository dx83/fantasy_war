#include "ProcUI_MiniMap.h"

#include "iStd.h"

#include "TextGroup.h"
#include "Proc.h"
#include "ProcUI_MapKey.h"
#include "ProcUI_Commander.h"

static MiniMap* mm;
iPopup** miniMap;

void drawPopTriBtn(float dt);
void drawPopMiniScreen(iPopup* popup, float dt, float rate);
void miniMapOpenEvent(iPopup* popup, float dt, float rate);

void createMiniMap()
{
	// initialize
	mm = new MiniMap;
	mm->off = getMapOffSet();

	mm->offPop = iPointMake(devSize.width - 400, devSize.height - 225);
	mm->offPrev = iPointZero;

	mm->tex = createImage("assets/map/game_map_mini.png");
	mm->dI.width = mm->tex->width - mm->off->mX / 2.5;
	mm->dI.height = mm->tex->height - mm->off->mY / 2.5;
	mm->rx = mm->ry = 190 / mm->dI.height;
	mm->sc.width = mm->dI.width * mm->rx;
	mm->sc.height = mm->dI.height * mm->ry;
	mm->bx.width = devSize.width * mm->rx / 4 + 3;
	mm->bx.height = devSize.height * mm->ry / 4;

	mm->bMove = false;
	mm->bClick = false;
	mm->bHide = false;

	mm->r = 0.f;
	mm->p = 0.f;
	mm->bEnd = false;

	// popup miniMap
    miniMap = new iPopup*[2];

	iImage* img;
	Texture* tex;
	iGraphics* g = iGraphics::share();

	// miniMap[0] Title
	iPopup* pop = new iPopup(1);
	iSize size = iSizeMake(400, 26);
	g->init(size);

	setRGBA(51, 76, 76, 128);
	g->fillRect(0, 0, size.width, size.height, 10);

	setStringName("굴림");
	setStringSize(20);
	//setStringRGBA(204, 25, 76, 255);
	setStringRGBA(0, 0, 0, 255);
	setStringBorder(1);
	//setStringBorderRGBA(25, 25, 25, 255);
	setStringBorderRGBA(0, 0, 0, 255);
	g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, mapName);
	tex = g->getTexture();

	img = new iImage();
	img->addObject(tex);
	freeImage(tex);
	pop->addObject(img);

	pop->style = iPopupNone;
	pop->openPoint = iPointZero;
	pop->closePoint = iPointZero;
	pop->methodTogether = drawPopMiniScreen;
	pop->methodFigure[0] = drawPopTriBtn;
	miniMap[0] = pop;

	// miniMap[1] layout
	pop = new iPopup();
	size = iSizeMake(400, 225);
	g->init(size);

	setRGBA(45, 77, 77, 128);
	g->fillRect(0, 0, size.width, size.height);

	tex = g->getTexture();
	img = new iImage();
	img->addObject(tex);
	freeImage(tex);
	img->position = iPointMake(0, 27);
	pop->addObject(img);

	// miniMap[1] screen
	img = new iImage();
	img->addObject(mm->tex);
    freeImage(mm->tex);
	img->position = iPointMake(6, 31);
	img->ix = mm->off->mX / 4;
	img->iy = mm->off->mY / 4;
	img->iw = mm->dI.width;
	img->ih = mm->dI.height;
	img->rx = mm->rx;
	img->ry = mm->ry;
	pop->addObject(img);
	mm->rx /= 4, mm->ry /= 4;	// original map / 4 = mini map

	// miniMap[1] setup
	pop->style = iPopupMove;
	pop->openPoint = iPointMake(0, -225);
	pop->closePoint = iPointMake(0, 0);
	pop->methodOpen = miniMapOpenEvent;
	miniMap[1] = pop;

	resetRGBA();
}

void freeMiniMap()
{
    //freeImage(mm->tex);
	delete mm;

	for (int i = 0; i < 2; i++)
		delete miniMap[i];
	delete miniMap;
}

void drawPopTriBtn(float dt)
{
	iPopup* pop = miniMap[1];
	if(mm->bHide)
    {
        setRGBA(52, 52, 52, 255);
        drawTri(triBtnX, triBtnY, triBtnW, triBtnH, REVERSE_NONE);
        setRGBA(102, 0, 0, 255);
        fillTri(triBtnX, triBtnY, triBtnW, triBtnH, REVERSE_NONE);
        pop->show(false);
    }
	else
    {
        setRGBA(52, 52, 52, 255);
        drawTri(triBtnX, triBtnY, triBtnW, triBtnH, REVERSE_HEIGHT);
        setRGBA(102, 0, 0, 255);
        fillTri(triBtnX, triBtnY, triBtnW, triBtnH, REVERSE_HEIGHT);
        pop->show(true);
    }

	resetRGBA();
}

void drawChooseBox(iPopup* pop)
{
	mm->off = getMapOffSet();
	
	float x = 6 - mm->off->curr.x * mm->rx;
	float y = 31 - mm->off->curr.y * mm->ry + pop->popPt.y;
	setLineWidth(2.0f);
	setRGBA(255, 255, 0, 200);
	drawRect(x+1, y+1, mm->bx.width-2, mm->bx.height-2);
	setRGBA(0, 255, 0, 153);
	fillRect(x+1, y+1, mm->bx.width-1, mm->bx.height-1);

	resetRGBA();
}

void drawPopMiniScreen(iPopup* popup, float dt, float rate)
{
	iPopup* pop = miniMap[1];
#if 0
	pop->openPoint += offMiniPop;
	pop->closePoint += offMiniPop;
	setClip(pop->closePoint.x, pop->closePoint.y, 400, 225);
	pop->paint(dt / 2);
	setClip(0, 0, 0, 0);
	pop->openPoint -= offMiniPop;
	pop->closePoint -= offMiniPop;
#else
	static Texture* tex = createTexture(400, 225);
	Texture* texBackup = texFboForiPopup;
	texFboForiPopup = tex;
	pop->paint(dt);//minimap
	texFboForiPopup = texBackup;
#endif
	drawChooseBox(pop);
}

void miniMapOpenEvent(iPopup* popup, float dt, float rate)
{
	if (popup->stat == iPopupOpen)
	{
		if(!mm->bEnd)		// 초기값 한번만 입력
			mm->p = devSize.height - mm->offPop.y;
		if (mm->p < 225)
		{
			mm->bEnd = true;
			mm->r = rate;
		}
		else
			mm->r = 0.0f;	// 끝나면 비율 초기화
	}
}

void drawMiniMap(float dt)
{
	if (mm->bEnd && mm->p < 225)
	{
		int t = 225 - mm->p;	// 역으로 올라갈 거리
		if (mm->r > 0.8f)		// 끝날때즈음 위치 정위치
		{
			mm->bEnd = false;
			mm->offPop.y = devSize.height - 225;
		}
		else
			mm->offPop.y = (devSize.height - mm->p) - (t * mm->r);
	}

	iPopup* pop = miniMap[0];
	iPoint o = pop->openPoint;// 실수형의 소수점 오차가 발생
	iPoint c = pop->closePoint;
	pop->openPoint += mm->offPop;
	pop->closePoint += mm->offPop;
	pop->paint(dt);
	pop->openPoint = o;//pop->openPoint -= mm->offPop;
	pop->closePoint = c;//pop->closePoint -= mm->offPop;

#if 0//충돌 범위 확인용
	iRect rt = iRectMake(mm->offPop.x + triBtnX, mm->offPop.y + triBtnY, triBtnW, triBtnH);
#if (OS==OS_ANDROID)
	rt.origin -= iPointMake(30, 25);
	rt.size += iSizeMake(60, 35);
#endif
	setRGBA(0, 0, 255, 180);
	fillRect(rt);

	rt = iRectMake(mm->offPop.x + 2, mm->offPop.y, 400, 25);
#if (OS==OS_ANDROID)
	rt.origin -= iPointMake(10, 20);
	rt.size += iSizeMake(20, 30);
#endif
	setRGBA(0, 255, 255, 180);
	fillRect(rt);
#endif
}

void showMiniMap(bool show)
{
	miniMap[0]->show(show);
}

bool keyMiniMap(iKeyStat stat, iPoint point)
{
	if (movingMapkey() ||
		movingCommander())
		return false;

	int currIdx = currentIndex;
	switch (stat)
	{
	case iKeyStatBegan:
	{
		iRect rt = iRectMake(mm->offPop.x + triBtnX, mm->offPop.y + triBtnY, triBtnW, triBtnH);
#if (OS==OS_ANDROID) // 충돌 범위 확대
		rt.origin -= iPointMake(30, 25);
		rt.size += iSizeMake(60, 35);
#endif
		if (containPoint(point, rt))// TriBtn
		{
			drawOrder(1);
			mm->bHide = !mm->bHide;
			break;
		}

		rt = iRectMake(mm->offPop.x + 2, mm->offPop.y, 400, 25);
#if (OS==OS_ANDROID) // 충돌 범위 확대
		rt.origin -= iPointMake(10, 20);
		rt.size += iSizeMake(20, 30);
#endif
		if (containPoint(point, rt))// miniMap Title
		{
			drawOrder(1);
			mm->bMove = true;
			mm->offPrev = point;
			break;
		}

		rt = iRectMake(mm->offPop.x + 6, mm->offPop.y + 31, mm->sc.width, mm->sc.height);
		if (mm->bHide == false && containPoint(point, rt))// miniMap Click
		{
			drawOrder(1);
			mm->bClick = true;
			gotoPosition(point, mm->offPop);
			break;
		}
		break;
	}

	case iKeyStatMoved:
	{
		if (mm->bMove)	// miniMap Pop Move
		{
			mm->offPop += point - mm->offPrev;
			mm->offPrev = point;
			float maxY;
			mm->bHide ? maxY = 26 : maxY = 225;
			restrictMap(mm->offPop, iPointZero,	iPointMake(devSize.width, devSize.height), 400, maxY);
		}
		else if (mm->bClick)// miniMap ChooseBox Drag
		{
			gotoPosition(point, mm->offPop);
		}
		break;
	}

	case iKeyStatEnded:
		mm->bMove = false;
		mm->bClick = false;
		break;
	}

	float maxY;
	mm->bHide ? maxY = 26 : maxY = 225;
	iRect rt = iRectMake(mm->offPop.x, mm->offPop.y, 400, maxY);
	if (containPoint(point, rt)) // currentIndex 유지
	{
		currentIndex = currIdx;
		return true;
	}

	return false;
}

bool movingMinimap()
{
	return mm->bMove || mm->bClick;
}

void gotoPosition(iPoint dst, iPoint offPop)
{
	iPoint p;
	if (offPop == iPointZero)// goto point
	{
		p.x = dst.x - devSize.width / 2;
		p.y = dst.y - devSize.height / 2;
	}
	else// on MiniMap
	{
		p.x = (dst.x - (offPop.x + 6)) / mm->rx - devSize.width / 2;
		p.y = (dst.y - (offPop.y + 31)) / mm->ry - devSize.height / 2;
	}
	setMapOffCurr(iPointZero - p);
	mapOffSet* offSet = getMapOffSet();
	setMapOffCurr(restrictMap(offSet->curr, offSet->min, offSet->max));
}

