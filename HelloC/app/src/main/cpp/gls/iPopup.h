#pragma once

#include "iArray.h"
#include "iImage.h"

extern Texture* texFboForiPopup;

enum iPopupStat
{
	iPopupOpen = 0,
	iPopupProc,
	iPopupClose
};

enum iPopupStyle
{
	iPopupNone = 0,
	iPopupAlpha,
	iPopupMove,
	iPopupZoom,
	iPopupTong,
	iPopupRotate,
	iPopupClash,
};

extern class iPopup;
typedef void (*Pop_OpenClose_Method)(iPopup* pop, float dt, float rate);
typedef void (*Pop_Draw_Method)(iPopup* pop, float dt, float rate);
typedef void (*Pop_Draw_Figure)(float dt);

class iPopup
{
public:
	iPopup(int figure = 0);
	virtual ~iPopup();

	static void freeImg(void* parm);
	void addObject(iImage* img);

	void paint(float dt);

	void show(bool show);

private:
	void popupStyle(float dt);

public:
	iArray* arrayImg;
	//figure
	int figure;
	Pop_Draw_Figure* methodFigure;
	//iPopup
	bool bShow;
	iPopupStyle style;
	iPopupStat stat;
	iPoint openPoint, closePoint;
	float aniDt, _aniDt;
	// Function
	Pop_OpenClose_Method methodOpen;
	Pop_OpenClose_Method methodClose;
	Pop_Draw_Method methodBefore;
	Pop_Draw_Method methodTogether;
	Pop_Draw_Method methodAfter;

	int8 selected;
	// paint
	float alpha;
	iPoint popPt;
	float rx, ry;
	float degree;
};



