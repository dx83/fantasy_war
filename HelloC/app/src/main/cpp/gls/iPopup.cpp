#include "iPopup.h"

#include "iStd.h"

Texture* texFboForiPopup;

iPopup::iPopup(int figure)
{
	arrayImg = new iArray(freeImg);

	if (figure)
	{
		this->figure = figure;
		methodFigure = new Pop_Draw_Figure[this->figure];
	}
	else
	{
		this->figure = 0;
		methodFigure = NULL;
	}

	bShow = false;
	style = iPopupNone;
	stat = iPopupClose;
	openPoint = iPointZero;
	closePoint = iPointZero;
	_aniDt = 0.2f;
	aniDt = 0.0f;

	methodOpen = NULL;
	methodClose = NULL;

	methodBefore = NULL;
	methodTogether = NULL;
	methodAfter = NULL;

	selected = -1;

	alpha = 1.0f;
	popPt = closePoint;
	rx = ry = 1.0f;
	degree = 0;
}

iPopup::~iPopup()
{
	delete arrayImg;
}

void iPopup::freeImg(void* parm)
{
	delete (iImage*)parm;
}

void iPopup::addObject(iImage* img)
{
	arrayImg->addObject(img);
}

void iPopup::paint(float dt)
{
	//dt /= 10;
	if (bShow == false)
		return;

	alpha = 1.0f;
	popPt = closePoint;
	rx = 1.0f, ry = 1.0f;
	degree = 0;


	if (methodBefore)
		methodBefore(this, dt, aniDt / _aniDt);

	popupStyle(dt);
#if 0
	//setRGBA(1, 1, 1, alpha);
	setAlpha(ca);
	for (int i = 0; i < arrayImg->count; i++)
	{
		iImage* img = (iImage*)arrayImg->objectAtIndex(i);
		//img->paint(dt, p.x, p.y, rx, ry);
		Texture* tex = (Texture*)img->arrayTex->objectAtIndex(img->frame);
		drawImage(tex,	p.x + img->position.x * rx,
						p.y + img->position.y * ry, TOP | LEFT,
			0, 0, tex->width, tex->height,
			rx, ry, 2, 0);
	}
	resetRGBA();
#else
	iPoint off = iPointZero;
	for (int i = 0; i < arrayImg->count; i++)
	{	//@ fbo(0,0) 기준에서 잘려나간 이미지의 off값 계산
		iImage* img = (iImage*)arrayImg->objectAtIndex(i);
		//img->paint(dt, 0, 0, 1.0f, 1.0f);
		if (off.x > img->position.x)
			off.x = img->position.x;	//@ off -
		if (off.y > img->position.y)
			off.y = img->position.y;	//@ off -
	}

	fbo->bind(texFboForiPopup);
	fbo->clear(0, 0, 0, 0);
	for (int i = 0; i < arrayImg->count; i++)
	{	//@ fbo(0,0) 기준에서 잘려나간 이미지를 보이도록 안으로 땡김
		iImage* img = (iImage*)arrayImg->objectAtIndex(i);
		img->paint(dt, -off.x, -off.y, 1.0f, 1.0f);	//@ off + 로 만들어서 그림
	}
	if (methodFigure)
	{
		for (int i = 0; i < figure; i++)
			methodFigure[i](dt);
	}
	if (methodTogether)
		methodTogether(this, dt, aniDt / _aniDt);
	fbo->unbind();
	popPt += off;	//@ 그리고 다시 fbo(0,0)을 뒤로 당기면 의도대로 그려짐
	Texture* t = texFboForiPopup;
	setGLBlend(GLBlendMultiplied);
	
	if (degree == 0.0f)
	{
		drawImage(t, popPt.x, popPt.y, TOP | LEFT,
			0, 0, t->width, t->height, rx, ry, 2, 0, REVERSE_HEIGHT);
	}
	else// 회전
	{
		// degree == 0.0f 와 동일한 효과
		//popPt.x += (t->width / 2 * rx);
		//popPt.y += (t->height / 2 * ry);
		//drawImage(t, p.x, p.y, VCENTER | HCENTER,
		//	0, 0, t->width, t->height, rx, ry, 2, 0, REVERSE_HEIGHT);
		popPt.x += (t->width / 2 * rx);
		popPt.y += (t->height / 2 * ry);
		drawImage(t, popPt.x, popPt.y, VCENTER | HCENTER,
			0, 0, t->width, t->height, rx, ry, 2, degree, REVERSE_HEIGHT);
	}
	
	setGLBlend(GLBlendAlpha);
#endif
	if (methodAfter)
		methodAfter(this, dt, aniDt / _aniDt);
}

void iPopup::show(bool show)
{
	if (show)
	{
		if (stat != iPopupClose)
			return;

		bShow = true;
		stat = iPopupOpen;
		aniDt = 0.0f;
	}
	else// if (show == false)
	{
		if (stat != iPopupProc)
			return;

		stat = iPopupClose;
		aniDt = 0.0f;
	}
}

void iPopup::popupStyle(float dt)
{
	switch (style)
	{
	case iPopupNone:
	{
		if (stat == iPopupOpen)
		{
			stat = iPopupProc;
			if (methodOpen)
				methodOpen(this, dt, aniDt / _aniDt);
		}
		else if (stat == iPopupProc)
		{
		}
		else if (stat == iPopupClose)
		{
			bShow = false;
			if (methodClose)
				methodClose(this, dt, aniDt / _aniDt);
		}
		break;
	}
	case iPopupAlpha:
	{
		if (stat == iPopupOpen)
		{
			alpha = aniDt / _aniDt;
			uint8 ca = (uint8)(255 * alpha);
			setAlpha(ca);
			aniDt += dt;
			if (aniDt >= _aniDt)
			{
				stat = iPopupProc;
				if (methodOpen)
					methodOpen(this, dt, aniDt / _aniDt);
			}
		}
		else if (stat == iPopupProc)
		{
		}
		else if (stat == iPopupClose)
		{
			alpha = 1.0f - aniDt / _aniDt;
			uint8 ca = (uint8)(255 * alpha);
			setAlpha(ca);
			aniDt += dt;
			if (aniDt >= _aniDt)
			{
				bShow = false;
				if (methodClose)
					methodClose(this, dt, aniDt / _aniDt);
			}
		}
		break;
	}
	case iPopupMove:
	{
		if (stat == iPopupOpen)
		{
			//popPt = linear(aniDt / _aniDt, openPoint, closePoint);
			popPt = easeIn(aniDt / _aniDt, openPoint, closePoint);
			//popPt = easeOut(aniDt / _aniDt, openPoint, closePoint);

			aniDt += dt;

			if (methodOpen)
				methodOpen(this, dt, aniDt / _aniDt);

			if (aniDt >= _aniDt)
			{
				stat = iPopupProc;
			}
		}
		else if (stat == iPopupProc)
		{
		}
		else if (stat == iPopupClose)
		{
			//popPt= linear(aniDt / _aniDt, closePoint, openPoint);
			popPt = easeIn(aniDt / _aniDt, closePoint, openPoint);
			//popPt = easeOut(aniDt / _aniDt, closePoint, openPoint);

			aniDt += dt;
			if (aniDt >= _aniDt)
			{
				bShow = false;
				if (methodClose)
					methodClose(this, dt, aniDt / _aniDt);
			}
		}
		break;
	}
	case iPopupZoom:
	{
		if (stat == iPopupOpen)
		{
			float r = linear(aniDt / _aniDt, 0, 1);
			//float r = easeIn(aniDt / _aniDt, 0, 1);
			//float r = easeOut(aniDt / _aniDt, 0, 1);
			rx = r;
			ry = r;
			popPt = linear(r, openPoint, closePoint);
			aniDt += dt;
			if (aniDt >= _aniDt)
			{
				stat = iPopupProc;
				if (methodOpen)
					methodOpen(this, dt, aniDt / _aniDt);
			}
		}
		else if (stat == iPopupProc)
		{
		}
		else if (stat == iPopupClose)
		{
			float r = linear(aniDt / _aniDt, 0, 1);
			//float r = easeIn(aniDt / _aniDt, 0, 1);
			//float r = easeOut(aniDt / _aniDt, 0, 1);
			rx =
			ry = 1.0f - r;
			popPt = linear(r, closePoint, openPoint);

			aniDt += dt;
			if (aniDt >= _aniDt)
			{
				bShow = false;
				if (methodClose)
					methodClose(this, dt, aniDt / _aniDt);
			}
		}
		break;
	}
	case iPopupTong:
		break;
	case iPopupRotate:
	{
		if (stat == iPopupOpen)
		{
			float r = linear(aniDt / _aniDt, 0, 1);
			rx = r;
			ry = r;
			popPt = linear(r, openPoint, closePoint);
			degree = r * 360;

			aniDt += dt;
			if (aniDt >= _aniDt)
			{
				stat = iPopupProc;
				if (methodOpen)
					methodOpen(this, dt, aniDt / _aniDt);
			}
		}
		else if (stat == iPopupProc)
		{
		}
		else if (stat == iPopupClose)
		{
			float r = linear(aniDt / _aniDt, 0, 1);
			rx =
				ry = 1.0f - r;
			popPt = linear(r, closePoint, openPoint);
			degree = r * 360;

			aniDt += dt;
			if (aniDt >= _aniDt)
			{
				bShow = false;
				if (methodClose)
					methodClose(this, dt, aniDt / _aniDt);
			}
		}
		break;
	}
	case iPopupClash:
	{
		if (stat == iPopupOpen)
		{
			//popPt = linear(aniDt / _aniDt, openPoint, closePoint);
			popPt = easeIn(aniDt / _aniDt, openPoint, closePoint);
			//popPt = easeOut(aniDt / _aniDt, openPoint, closePoint);

			aniDt += dt / 4;

			if (aniDt >= _aniDt)
			{
				stat = iPopupProc;
				if (methodOpen)
					methodOpen(this, dt, aniDt / _aniDt);
			}
		}
		else if (stat == iPopupProc)
		{
		}
		else if (stat == iPopupClose)
		{
			//popPt= linear(aniDt / _aniDt, closePoint, openPoint);
			popPt = easeIn(aniDt / _aniDt, closePoint, openPoint);
			//popPt = easeOut(aniDt / _aniDt, closePoint, openPoint);

			aniDt += dt / 4;
			if (aniDt >= _aniDt)
			{
				bShow = false;
				if (methodClose)
					methodClose(this, dt, aniDt / _aniDt);
			}
		}
		break;
	}
	}
}

