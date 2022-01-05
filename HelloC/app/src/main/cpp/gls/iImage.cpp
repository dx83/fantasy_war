#include "iImage.h"

#include "iStd.h"

iImage::iImage()
{
	arrayTex = new iArray(freeObject);

	ani = false;
	repeatNum = 0;
	repeatIdx = 0;
	frame = 0;
	lastFrame = false;
	_delta = 0.08f;
	delta = 0.0f;

	position = iPointZero;
	anc = TOP | LEFT;
	ix = 0, iy = 0;
	iw = 0, ih = 0;
	rx = 1, ry = 1;

	reverse = REVERSE_NONE;

	method = NULL;
	parm = NULL;
}

iImage::~iImage()
{
	delete arrayTex;
}

void iImage::freeObject(void* parm)
{
	freeImage((Texture*)parm);
}

void iImage::addObject(Texture* tex)
{
	arrayTex->addObject(tex);
	tex->retainCount++;
	if (this->tex == NULL)
		this->tex = tex;
}

void iImage::setTexObject(int index)
{
	tex = (Texture*)arrayTex->objectAtIndex(index);
	frame = index;
}

iRect iImage::touchRect(iPoint off, iSize size)
{
	iRect rt;
	rt.origin = position + off - iPointMake(size.width / 2, size.height / 2);

	switch (anc)
	{
	case LEFT | TOP:																			break;
	case LEFT | VCENTER:										rt.origin.y -= tex->height / 2;	break;
	case LEFT | BOTTOM:											rt.origin.y -= tex->height;		break;

	case HCENTER | TOP:			rt.origin.x -= tex->width / 2;									break;
	case HCENTER | VCENTER:		rt.origin.x -= tex->width / 2;	rt.origin.y -= tex->height / 2;	break;
	case HCENTER | BOTTOM:		rt.origin.x -= tex->width / 2;	rt.origin.y -= tex->height;		break;

	case RIGHT | TOP:			rt.origin.x -= tex->width;										break;
	case RIGHT | VCENTER:		rt.origin.x -= tex->width;		rt.origin.y -= tex->height / 2;	break;
	case RIGHT | BOTTOM:		rt.origin.x -= tex->width;		rt.origin.y -= tex->height;		break;
	}

	rt.size = iSizeMake(tex->width, tex->height) + size;

	return rt;
}

void iImage::startAnimation(ANIMATION_METHOD m, void* p)
{
	ani = true;
	//repeatNum;// 밖에서 설정
	repeatIdx = 0;
	frame = 0;
	//_delta;// 밖에서 설정
	delta = 0.0f;

	method = m;
	parm = p;
}

void iImage::stopAnimation()
{
	ani = false;
}

void iImage::paint(float dt, iPoint off, iPoint rate)
{
	paint(dt, off.x, off.y, rate.x, rate.y);
}
void iImage::paint(float dt, float x, float y, float sx, float sy)
{
	if (ani)
	{
		delta += dt;
		if (delta > _delta)
		{
			//delta -= _delta;
			delta = 0.0f;

			frame++;
			if (frame > arrayTex->count - 1)
			{
				frame = 0;
				if (repeatNum == 0)// inf
				{
					// 
				}
				else
				{
					repeatIdx++;
					if (repeatIdx == repeatNum)
					{
						if( method )
							method(parm);
						ani = false;

						if (lastFrame)
							frame = arrayTex->count - 1;
					}
				}
			}
		}
	}

	tex = (Texture*)arrayTex->objectAtIndex(frame);
	if (iw == 0)	iw = tex->width;
	if (ih == 0)	ih = tex->height;
	drawImage(tex, position.x + x, position.y + y, anc,
		ix, iy, iw, ih,
		rx * sx, ry * sy, 2, 0, reverse);
}

iImage* iImage::copy()
{
	iImage* img = new iImage();
	iArray* a = img->arrayTex;

	memcpy(img, this, sizeof(iImage));
	img->arrayTex = a;
	for (int i = 0; i < arrayTex->count; i++)
		img->addObject((Texture*)arrayTex->objectAtIndex(i));

	return img;
}

