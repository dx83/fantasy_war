#include "iRect.h"

iRect iRectMake(float x, float y, float width, float height)
{
	iRect p;
	p.origin.x = x;
	p.origin.y = y;
	p.size.width = width;
	p.size.height = height;
	return p;
}

bool containPoint(iPoint point, iRect rect)
{	// point가 rect밖이 아닌 경우
	return !(
		point.x < rect.origin.x ||
		point.x > rect.origin.x + rect.size.width ||
		point.y < rect.origin.y ||
		point.y > rect.origin.y + rect.size.height);
}

bool containRect(iRect rt0, iRect rt1)
{	// rt0가 rt1 밖이 아닌 경우
	return !(
		rt0.origin.x + rt0.size.width < rt1.origin.x ||		// rt0의 맨오른쪽이 rt1의 밖인지
		rt0.origin.x > rt1.origin.x + rt1.size.width ||		// rt1의 맨오른쪽이 rt0의 밖인지
		rt0.origin.y + rt0.size.height < rt1.origin.y ||	// rt0의 맨아래쪽이 rt1의 밖인지
		rt0.origin.y > rt1.origin.y + rt1.size.height);		// rt1의 맨아래쪽이 rt0의 밖인지
}

