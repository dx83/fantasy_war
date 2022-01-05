#pragma once

#include "iType.h"

#define DISPLAY_FPS

class iFPS
{
public:
	static iFPS* instance();
private:
	iFPS();
public:
	virtual ~iFPS();

	float update();

	void setSlowTime(float delta, float delay);

public:

#if (OS==OS_WINDOWS)
	unsigned long now;
#elif (OS==OS_ANDROID)
	timeval now;
#endif

#ifdef DISPLAY_FPS
	int framesPerSec;
	float tickDt;
	int num;
#endif // DISPLAY_FPS
	float lastDt;

	bool ingSlowTime;
	float slowDelta, _slowDelta;
	float slowDelay, _slowDelay;
};

