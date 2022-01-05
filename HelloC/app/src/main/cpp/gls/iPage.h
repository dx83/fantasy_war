#pragma once

#include "iType.h"
#include "iControl.h"
#include "iRect.h"

typedef void (*Method_Page_Changed)(int page);
typedef void (*Method_Page_Draw)(float dt);

class iPage
{
public:
	iPage();
	virtual ~iPage();

	void init(	bool horizontal,
				int page,
				iRect rt,
				Method_Page_Changed mChanged,
				Method_Page_Draw mDrawBefore,
				Method_Page_Draw mDraw,
				Method_Page_Draw mDrawAfter);

	void paint(float dt, iPoint off);
	bool key(iKeyStat stat, iPoint point);

private:
	void restrictPage();

public:
	bool horizontal, down, right;
	int page, _page;
	iRect rt;
	bool touching, moved, keyPush;
	iPoint prevPoint;
#if (OS==OS_WINDOWS)
	DWORD prevTime;//unsigned long prevTime;
#elif (OS==OS_ANDROID)
	timeval prevTime;
#endif
	iPoint off;
	float force;

	Method_Page_Changed methodChanged;
	Method_Page_Draw methodDrawBefore;
	Method_Page_Draw methodDraw;
	Method_Page_Draw methodDrawAfter;
};

