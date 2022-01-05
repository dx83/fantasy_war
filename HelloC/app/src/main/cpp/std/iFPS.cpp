#include "iFPS.h"

#include "iStd.h"

iFPS* iFPS::instance()
{
	static iFPS* ins = new iFPS();
	return ins;
}

iFPS::iFPS()
{
#if (OS==OS_WINDOWS)
	now = GetTickCount();
#elif (OS==OS_ANDROID)
	gettimeofday(&now, NULL);
#endif

#ifdef DISPLAY_FPS
	framesPerSec = 0;
	tickDt = 0.0f;
	num = 0;
#endif
	lastDt = 0.0f;

	ingSlowTime = false;
	_slowDelta = 0.0f;
	slowDelta = 0.0f;
	_slowDelay = 0.0f;
	slowDelay = 0.0f;
}

iFPS::~iFPS()
{
}

float iFPS::update()
{
#if (OS==OS_WINDOWS)
	unsigned long n = GetTickCount();
	float dt = (n - now) / 1000.f;
#elif (OS==OS_ANDROID)
	timeval n;
	gettimeofday(&n, NULL);
	float dt = (n.tv_sec - now.tv_sec) + (n.tv_usec - now.tv_usec) / 1000000.0f;
#endif

#ifdef DISPLAY_FPS
	tickDt += dt;
	num++;
	if (tickDt > 0.3f)
	{
		framesPerSec = num / tickDt;

		tickDt = 0.0f;
		num = 0;
	}
#endif
	now = n;
	lastDt = dt;
	if (ingSlowTime)
	{
		if (slowDelta < _slowDelta)
		{
			float delta = dt * (1 - _sin(90 * slowDelta / _slowDelta));

			slowDelta += dt;
			if (slowDelta > _slowDelta)
				slowDelta = _slowDelta;
			dt = delta;
		}
		else if (slowDelta == _slowDelta)
		{
			slowDelay += dt;
			if (slowDelay > _slowDelay)
				slowDelta += 0.000001f;

			dt = 0.0f;
		}
		else if (slowDelta < _slowDelta * 2)
		{
			float r = (slowDelta - _slowDelta) / _slowDelta;
			float delta = dt * r * r;

			slowDelta += dt;
			dt = delta;
		}
		else
		{
			ingSlowTime = false;
		}
	}
	return dt;
}

void iFPS::setSlowTime(float delta, float delay)
{
	if (ingSlowTime)
		return;

	ingSlowTime = true;
	_slowDelta = delta;
	slowDelta = 0.0f;
	_slowDelay = delay;
	slowDelay = 0.0f;
}
