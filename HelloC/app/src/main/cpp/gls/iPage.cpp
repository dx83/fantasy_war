#include "iPage.h"

#include "iStd.h"

iPage::iPage()
{
	horizontal = down = right = true;
	page = _page = 0;
	rt = iRectZero;
	touching = moved = false;
	prevPoint = iPointZero;
	prevTime = iFPS::instance()->now;
	off = iPointZero;
	force = 0.0f;

	methodChanged = NULL;
	methodDrawBefore = NULL;
	methodDraw = NULL;
	methodDrawAfter = NULL;
}

iPage::~iPage()
{
}

void iPage::init(	bool horizontal,
					int page,
					iRect rt,
					Method_Page_Changed mChanged,
					Method_Page_Draw mDrawBefore,
					Method_Page_Draw mDraw,
					Method_Page_Draw mDrawAfter)
{
	this->horizontal = horizontal;
	_page = page;
	this->rt = rt;
	methodChanged = mChanged;
	methodDrawBefore = mDrawBefore;
	methodDraw = mDraw;
	methodDrawAfter = mDrawAfter;
}

void iPage::paint(float dt, iPoint p)
{
	if (methodDrawBefore)
		methodDrawBefore(dt);
	if (methodDraw)
		methodDraw(dt);
	if (methodDrawAfter)
		methodDrawAfter(dt);

	if (horizontal)
	{
		if (getKeyDown() & keysLeft)
		{
			if (page == 0)
				return;

			keyPush = true;
			right = false;
			force = 2000;
			page--;
			if (methodChanged)
				methodChanged(page);

		}
		else if (getKeyDown() & keysRight)
		{
			if (page == _page - 1)
				return;
			
			keyPush = true;
			right = true;
			force = -2000;
			page++;
			if (methodChanged)
				methodChanged(page);
		}

		if (keyPush)
		{
			off.x += force * dt;
			restrictPage();
			if (off.x == -rt.size.width * page ||	//right
				off.x == rt.size.width * page)
				keyPush = false;
		}
	}
	else
	{
		if (getKeyDown() & keysUp)
		{
			if (page == 0)
				return;

			keyPush = true;
			down = false;
			force = 2000;
			page--;
			if (methodChanged)
				methodChanged(page);

		}
		else if (getKeyDown() & keysDown)
		{
			if (page == _page - 1)
				return;

			keyPush = true;
			down = true;
			force = -2000;
			page++;
			if (methodChanged)
				methodChanged(page);
		}

		if (keyPush)
		{
			off.y += force * dt;
			restrictPage();
			if (off.y == -rt.size.height * page ||	//down
				off.y == rt.size.height * page)
				keyPush = false;
		}
	}

	if (touching || keyPush)
		return;

	int newPage = page;

	if (horizontal)
	{
		off.x += force * dt;

		int x = off.x * -1;
		int dx = rt.size.width;
		if (right)
			newPage = (x + dx / 1.3) / dx;
		else
			newPage = (x + dx / 4.35) / dx;

	}
	else
	{
		off.y += force * dt;

		int y = off.y * -1;
		int dy = rt.size.height;
		if (down)
			newPage = (y + dy / 1.3) / dy;
		else
			newPage = (y + dy / 4.35) / dy;
	}

	if (newPage != page)
	{
		if (newPage < 0)				newPage = 0;
		else if (newPage > _page - 1)	newPage = _page - 1;

		page = newPage;
		if (methodChanged)
			methodChanged(page);
	}

	if (force > 0)
	{
		force -= 500 * dt;
		if (force < 0)
			force = 0.0f;
		//printf("-force %f\n", force);
	}
	else if (force < 0)
	{
		force += 500 * dt;
		if (force > 0)
			force = 0.0f;
		//printf("+force %f\n", force);
	}
	else
	{
		//printf("force==0\n");
		iPoint tOff = iPointZero;
		if (horizontal)
		{
			tOff.x = -rt.size.width * page;
			if (off.x < tOff.x)
			{
				off.x += 1500 * dt;
				if (off.x > tOff.x)
					off.x = tOff.x;
			}
			else if (off.x > tOff.x)
			{
				off.x -= 1500 * dt;
				if (off.x < tOff.x)
					off.x = tOff.x;
			}
		}
		else
		{
			tOff.y = -rt.size.height * page;
			if (off.y < tOff.y)
			{
				off.y += 1500 * dt;
				if (off.y > tOff.y)
					off.y = tOff.y;
			}
			else if (off.y > tOff.y)
			{
				off.y -= 1500 * dt;
				if (off.y < tOff.y)
					off.y = tOff.y;
			}
		}
	}

	restrictPage();
}

void iPage::restrictPage()
{
	iPoint tOff = iPointZero;
	if (keyPush)
	{
		if (horizontal)
		{
			tOff.x = -rt.size.width * page;
			if (right && off.x < tOff.x)
			{
				force = 0.0f;
				off.x = tOff.x;
			}
			else if (!right && off.x > tOff.x)
			{
				force = 0.0f;
				off.x = tOff.x;
			}
		}
		else
		{
			tOff.y = -rt.size.height * page;
			if (down && off.y < tOff.y)
			{
				force = 0.0f;
				off.y = tOff.y;
			}
			else if (!down && off.y > tOff.y)
			{
				force = 0.0f;
				off.y = tOff.y;
			}
		}
	}

	if (horizontal)
	{
		tOff.x = -rt.size.width * (_page - 1);
		if (right && off.x < tOff.x)
		{
			force = 0.0f;
			off.x = tOff.x;
		}
		tOff.x = 0;
		if (!right && off.x > tOff.x)
		{
			force = 0.0f;
			off.x = tOff.x;
		}
	}
	else
	{
		tOff.y = -rt.size.height * (_page - 1);
		if (down && off.y < tOff.y)
		{
			force = 0.0f;
			off.y = tOff.y;
		}
		tOff.y = 0;
		if (!down && off.y > tOff.y)
		{
			force = 0.0f;
			off.y = tOff.y;
		}
	}
}

bool iPage::key(iKeyStat stat, iPoint point)
{
	if (containPoint(point, rt) == false)
		return false;

	switch (stat)
	{
	case iKeyStatBegan:
		touching = true;
		moved = false;
		prevPoint = point;
		prevTime = iFPS::instance()->now;
		break;
	case iKeyStatMoved:
		if (touching)
		{
			iPoint mp = point - prevPoint;
			if (iPointLength(mp) > 3)// if mobile, at 6
			{
				float move = horizontal ? mp.x : mp.y;
#if (OS==OS_WINDOWS)
				unsigned long now = iFPS::instance()->now;
				float delta = (now - prevTime) / 1000.f;
				force = move / dt * 0.5f;//감도
#elif (OS==OS_ANDROID)
				timeval now = iFPS::instance()->now;
				float delta = (now.tv_sec - prevTime.tv_sec) + (now.tv_usec - prevTime.tv_usec) / 1000.f;
				force = move / delta * 0.5f;//감도
#endif
				if(delta == 0.0f)
					force = 0.0f;				// sensitivity
				else
					force = move / delta * 0.5f;	// sensitivity
				//printf("force: %f\n", force);
				prevPoint = point;
				prevTime = iFPS::instance()->now;

				if (horizontal)
				{
					mp.x < 0 ? right = true : right = false;
					off.x += move;
				}
				else
				{
					mp.y < 0 ? down = true : down = false;
					off.y += move;
				}
				restrictPage();
				moved = true;
			}
		}
		break;
	case iKeyStatEnded:
		touching = false;
		//if (moved == false)
		//{
		//	printf("click\n");
		//}
		moved = false;
		break;
	}
	
	return true;
}

