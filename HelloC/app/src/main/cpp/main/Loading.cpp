#include "Loading.h"

int gameState = gs_intro, toGameState;
MethodLoad methodFree, methodLoad;
float loadingDt = 0.0f;
#define _loadingDt 1.0f

static int loadStep = 0, methodNum = 0, percent = 0;
// methodNum 1 : string
// methodNum 2 : loadingBar

void setLoading(int gameState, MethodLoad free, MethodLoad load, int methods)
{
	loadStep = 1;
    toGameState = gameState;
    methodFree = free;
    methodLoad = load;
    loadingDt = 0.000001f;

    methodNum = methods;
}

void drawLoading(float dt)
{
	// native-lib.cpp : if (dt > 0.1f) dt = 0.01f;//dt = 0.001f;
	if (loadingDt == 0.0f)
		return;

	float a = 1.0f;

	//loge("loadStep %d", loadStep);
	switch (loadStep)
	{
		case 1:// Dark
            //gameState == gs_proc ? loadingDt *= 3 : loadingDt += dt;
			loadingDt += dt;//@
			if (loadingDt < _loadingDt)// become dark
			{
				if (loadingDt > _loadingDt)
					loadingDt = _loadingDt;

				a = loadingDt / _loadingDt;
			}
			else if (loadingDt >= _loadingDt)
			{
				if (methodNum==1)		loadStrAppear();
				else if(methodNum==2)	loadProgress();
				//loge("free start");
				if (methodFree)
					methodFree();
#if CHECK_TEXTURE    // delete check
				loge("FREE:texture_num %d\n", texture_num);
#endif
				a = 1.0f;// whole dark

				gameState = gs_delay;
				loadingDt = 0.000001f;
				loadStep = 2;

				//loge("free END %d", methodNum);
			}
			break;

		case 2://methodFree <=> methodLoad between delay (0.75f 이상)
            loadingDt += dt;
            if (methodNum==2)
            	setProgress(loadingDt * 80);
			if (loadingDt >= _loadingDt)
			{
				//loge("loading 00 %d", methodLoad);
				if (methodLoad)
					methodLoad();
#if CHECK_TEXTURE    // how many use memory each scene
                loge("LOAD:texture_num %d\n", texture_num);
#endif
				gameState = toGameState;
				loadingDt = 0.000001f;
#if 0// string loading : 터치시 로딩화면 사라짐
				if (methodNum==1)   loadStep = 101;
				else                loadStep = 3;
#else
				loadStep = 3;
#endif
                //loge("loading 01");
			}
			break;

        case 3:// Loading Scene
        	if(methodNum)
			{
				loadingDt += dt;
				if (methodNum==2)
				{
					if (percent >= 100)
						loadingDt *= 1.5;
					else
						setProgress(percent + loadingDt * 80);
				}
				if (loadingDt > _loadingDt)
				{
					//gameState == gs_proc ? loadingDt = 0.2f : loadingDt = 0.000001f;
					loadingDt = 0.000001f;//@
#if 0
                    if(toGameState == gs_proc || toGameState == gs_menu)
                        loadingDt = 0.2f;
                    else
                        loadingDt = 0.000001f;
#endif
					methodNum = 0;
					if(methodNum==1)		freeStrAppear();
					else if(methodNum==2)	freeProgress();
				}
			}
        	else
            	loadStep = 4;
            break;

		case 4:// Scene Change
			//gameState == gs_proc ? loadingDt *= 4 : loadingDt += dt;
			loadingDt += dt;//@
#if 0
            if(toGameState == gs_proc || toGameState == gs_menu)
                loadingDt *= 2;
            else
                loadingDt += dt;
#endif
            if (loadingDt > _loadingDt)
            {
                //extern bool toMenu;
                //if (toMenu)
                //{
                //    toMenu = false;
                //    audioPlay(0);
                //}
                loadingDt = 0.0f;
                loadStep = 0;
                a = 0.0f;
                break;
            }
            a = 1.0f - (loadingDt - _loadingDt) / _loadingDt;// brighten
			break;
	}

	uint8 ca = (uint8) (255 * a);
	setRGBA(0, 0, 0, ca);
	fillRect(-10, -10, devSize.width + 20, devSize.height + 20);
	resetRGBA();

	if(loadStep == 2 || loadStep == 3 || loadStep == 101)
	{
		switch(methodNum)
		{
			case 1: drawStrAppear(); 	break;
			case 2: drawProgress(dt);	break;
		}
	}
}

bool keyLoading(iKeyStat state, iPoint point)
{
	if (loadStep == 101)//methodNum == 1
	{
		iRect rt = iRectMake(0, 0, devSize.width, devSize.height);
		if (containPoint(point, rt))
			loadStep = 3;
	}

	return loadStep == 3 || loadStep == 4;
}


// =================================================
// string appear
// =================================================
Texture* strContents;

void loadStrAppear()
{
	const char* str[2] =
	{
		"BUSAN GAME",
		"ACADEMY",
	};

    setStringName("assets/font/SEBANG_Gothic_Bold.ttf");
    setStringSize(150);//180
    setStringRGBA(217, 229, 255, 255);
    setStringBorder(5);
	setStringBorderRGBA(217, 229, 255, 255);

    iGraphics* g = iGraphics::share();
    iSize size = iSizeMake(1600, 600);
    g->init(size);
	g->drawString(size.width / 2, size.height / 2 - 100, VCENTER | HCENTER, "%s", str[0]);
    g->drawString(size.width / 2, size.height / 2 + 100, VCENTER | HCENTER, "%s", str[1]);
    strContents = g->getTexture();
}

void freeStrAppear()
{
	freeImage(strContents);
}

void drawStrAppear()
{
	if(strContents != NULL)
    	drawImage(strContents, (devSize.width - strContents->width) / 2, (devSize.height - strContents->height) / 2, TOP | LEFT);
}

// =================================================
// loadingbar
// =================================================
Texture** texProgressBar;
int progressMax;
int progressCurr;
iStrTex* stProgressPercent;
Texture* methodStProgressPercent(const char* str);

void loadProgress()
{
	texProgressBar = new Texture*[2];
	iGraphics* g = iGraphics::share();
	iSize size = iSizeMake(1000, 60);
	g->init(size);
	setRGBA(150, 150, 150, 200);
	g->fillRect(0, 0, size.width, size.height, size.height / 2);
	texProgressBar[0] = g->getTexture();

	size = iSizeMake(982, 42);
	g->init(size);
	setRGBA(61, 183, 204, 255);
	g->fillRect(0, 0, size.width, size.height, size.height / 2);
	texProgressBar[1] = g->getTexture();

	progressMax = 100;
	progressCurr = 0;

	percent = 0;
	stProgressPercent = new iStrTex(methodStProgressPercent);
	stProgressPercent->setString("0");
}

Texture* methodStProgressPercent(const char* str)
{
	iGraphics* g = iGraphics::share();
	iSize size = iSizeMake(200, 100);
	g->init(size);

	int percent = atoi(str);
	setStringSize(50);
	setStringRGBA(255, 255, 255, 255);
	g->drawString(size.width / 2, size.height / 2, VCENTER | HCENTER, "%d%%%%", percent);//가변인자 %% -> %

	return g->getTexture();
}

void freeProgress()
{
	for (int i = 0; i < 2; i++)
		freeImage(texProgressBar[i]);
	delete stProgressPercent;
}

void drawProgress(float dt)
{
	stProgressPercent->paint(devSize.width / 2, devSize.height / 2, VCENTER | HCENTER);

	drawImage(texProgressBar[0], devSize.width / 2, devSize.height / 2 + 80, VCENTER | HCENTER);

	Texture* t = texProgressBar[1];
	drawImage(t, devSize.width / 2 - t->width / 2, devSize.height / 2 + 80, VCENTER | LEFT,
		0, 0, t->width, t->height, percent * 0.01, 1, 2, 0);
}

void setProgress(int tick)
{
	progressCurr = tick;

	percent = 100.f * progressCurr / progressMax;
	if (percent > 100)
		percent = 100;
	stProgressPercent->setString("%d", percent);
}

