#include "iGraphics.h"

#include "iStd.h"

#if (OS==OS_WINDOWS)
iGraphics::iGraphics()
{
	bmp = NULL;
	g = NULL;
}

iGraphics* iGraphics::share()
{
	static iGraphics* graphics = NULL;
	if (graphics == NULL)
		graphics = new iGraphics();
	return graphics;
}

iGraphics::~iGraphics()
{
	if (bmp)
		delete bmp;
	if (g)
		delete g;
}

void iGraphics::init(iSize size)
{
	init(size.width, size.height);
}

void iGraphics::init(int width, int height)
{
	if (bmp)
	{
		delete bmp;
		delete g;
	}
	bmp = new Bitmap(width, height, PixelFormat32bppARGB);
	g = Graphics::FromImage(bmp);
	g->SetPageUnit(UnitPixel);
	//g->SetPageScale(1.0f);
	g->SetPixelOffsetMode(PixelOffsetModeHalf);
	g->SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
	g->SetTextContrast(0xffffffff);
	g->SetCompositingMode(CompositingModeSourceOver);
	g->SetCompositingQuality(CompositingQualityAssumeLinear);
	g->SetSmoothingMode(SmoothingModeAntiAlias8x8);
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);

	clear();
}

void iGraphics::clear(float r, float g, float b, float a)
{
	this->g->Clear(Color(a * 0xFF, r * 0xFF, g * 0xFF, b * 0xFF));
}
void iGraphics::clear()
{
	g->Clear(Color(0, 0, 0, 0));
}

Texture* iGraphics::getTexture()
{
	int width = bmp->GetWidth();
	int height = bmp->GetHeight();
	Rect rect(0, 0, width, height);
	BitmapData bmpData;
	bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

	int stride = bmpData.Stride / 4;
	uint32* rgba = (uint32*)bmpData.Scan0;

	int potWidth = nextPot(width);
	int potHeight = nextPot(height);
	uint32* pixel = new uint32[potWidth * potHeight];
	copyBmpToRgba(rgba, stride, 0, 0, width, height,
		pixel, potWidth, potHeight);

	Texture* tex = createImageWithRGBA((uint8*)pixel, width, height);
	delete pixel;

	bmp->UnlockBits(&bmpData);
	delete bmp;
	bmp = NULL;
	delete g;
	g = NULL;

	return tex;
}

static ULONG_PTR gdiplusToken;
void iGraphics::start(HDC hdc)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}
void iGraphics::end()
{
	GdiplusShutdown(gdiplusToken);
}

void iGraphics::drawLine(iPoint sp, iPoint ep)
{
	drawLine(sp.x, sp.y, ep.x, ep.y);
}
void iGraphics::drawLine(float x0, float y0, float x1, float y1)
{
	float r, g, b, a;
	getRGBA(r, g, b, a);
	float lineWidth = getLineWidth();

	Pen pen(Color(floatToUint8(a),
		floatToUint8(r),
		floatToUint8(g),
		floatToUint8(b)));
	pen.SetWidth(lineWidth);
	this->g->DrawLine(&pen, x0, y0, x1, y1);
}

void iGraphics::drawRect(iRect rt, float radius)
{
	drawRect(rt.origin.x, rt.origin.y, rt.size.width, rt.size.height, radius);
}
void iGraphics::drawRect(float x, float y, float width, float height, float radius)
{
	float r, g, b, a;
	getRGBA(r, g, b, a);
	float lineWidth = getLineWidth();

	Pen pen(Color(floatToUint8(a),
		floatToUint8(r),
		floatToUint8(g),
		floatToUint8(b)));
	pen.SetWidth(lineWidth);

	GraphicsPath path;
	path.AddLine(x + radius, y, x + width - (radius * 2), y);
	path.AddArc(x + width - (radius * 2), y, radius * 2, radius * 2, 270, 90);
	path.AddLine(x + width, y + radius, x + width, y + height - (radius * 2));
	path.AddArc(x + width - (radius * 2), y + height - (radius * 2), radius * 2, radius * 2, 0, 90);
	path.AddLine(x + width - (radius * 2), y + height, x + radius, y + height);
	path.AddArc(x, y + height - (radius * 2), radius * 2, radius * 2, 90, 90);
	path.AddLine(x, y + height - (radius * 2), x, y + radius);
	path.AddArc(x, y, radius * 2, radius * 2, 180, 90);
	path.CloseFigure();

	this->g->DrawPath(&pen, &path);
}

void iGraphics::fillRect(iRect rt, float radius)
{
	fillRect(rt.origin.x, rt.origin.y, rt.size.width, rt.size.height, radius);
}
void iGraphics::fillRect(float x, float y, float width, float height, float radius)
{
	float r, g, b, a;
	getRGBA(r, g, b, a);
	SolidBrush brush(Color(floatToUint8(a),
		floatToUint8(r),
		floatToUint8(g),
		floatToUint8(b)));

	GraphicsPath path;
	path.AddLine(x + radius, y, x + width - (radius * 2), y);
	path.AddArc(x + width - (radius * 2), y, radius * 2, radius * 2, 270, 90);
	path.AddLine(x + width, y + radius, x + width, y + height - (radius * 2));
	path.AddArc(x + width - (radius * 2), y + height - (radius * 2), radius * 2, radius * 2, 0, 90);
	path.AddLine(x + width - (radius * 2), y + height, x + radius, y + height);
	path.AddArc(x, y + height - (radius * 2), radius * 2, radius * 2, 90, 90);
	path.AddLine(x, y + height - (radius * 2), x, y + radius);
	path.AddArc(x, y, radius * 2, radius * 2, 180, 90);
	path.CloseFigure();

	this->g->FillPath(&brush, &path);
}

igImage* iGraphics::createIgImage(const char* szFormat, ...)
{
	char szText[1024];
	va_start_end(szText, szFormat);

	wchar_t* ws = utf8_to_utf16(szText);
	Image* img = Image::FromFile(ws, false);
	delete ws;

	return img;
}
void iGraphics::freeIgImage(igImage* img)
{
	delete img;
}

void iGraphics::drawIgImage(igImage* img, float x, float y, int anc,
	float ix, float iy, float iw, float ih,
	float rx, float ry,
	int xyz, float degree)
{
#if 0
	Texture tex = {
		img,
		img->GetWidth(),
		img->GetHeight(),
		img->GetWidth(),
		img->GetHeight(),
		1
	};
	pushGraphics();
	drawImage(&tex, x, y, anc,
		ix, iy, iw, ih,
		rx, ry,
		xyz, degree);
	popGraphics();
#endif
}

void iGraphics::drawIgImage(igImage* img, float x, float y, int anc)
{
	drawIgImage(img, x, y, TOP | LEFT,
		0, 0, img->GetWidth(), img->GetHeight(),
		1.0f, 1.0f,
		2, 0);
}

void checkFontFamily(FontFamily* ff, int& fontStyle)
{
	const char* path = getStringName();
	if (path[0] != 'a' || path[1] != 's' || path[2] != 's')// assets/
	{
		wchar_t* wstr = utf8_to_utf16(path);
		FontFamily f(wstr);
		delete wstr;
		fontStyle = FontStyleRegular;

		memcpy(ff, &f, sizeof(FontFamily));
	}
	else
	{
		PrivateFontCollection* pfc = new PrivateFontCollection();

		wchar_t* wstr = utf8_to_utf16(getStringName());
		pfc->AddFontFile(wstr);
		delete wstr;

		int count = pfc->GetFamilyCount();
		int found;
		pfc->GetFamilies(count, ff, &found);

		delete pfc;

		for (int i = 0; i < found; i++)
		{
			//ff->IsAvailable();
			if (ff->IsStyleAvailable(FontStyleRegular))
			{
				fontStyle = FontStyleRegular;
				return;
			}
			else if (ff->IsStyleAvailable(FontStyleBold))
			{
				fontStyle = FontStyleBold;
				return;
			}
		}
	}
}

class StringSize
{
public:
	StringSize()
	{
		bmp = new Bitmap(devSize.width, devSize.height, PixelFormat32bppARGB);
		g = Graphics::FromImage(bmp);
	}
	virtual ~StringSize()
	{
		delete bmp;
		delete g;
	}

	iRect rect(const char* szText)
	{
#if 0// infite loop
		float br, bg, bb, ba; getStringRGBA(br, bg, bb, ba);
		float cr, cg, cb, ca; getStringBorderRGBA(cr, cg, cb, ca);
		setStringRGBA(1, 1, 1, 1);
		setStringBorderRGBA(0, 0, 0, 1);
		Graphics* tmp = graphics;
		graphics = g;

		clearRect();
		drawString(0, 0, TOP | LEFT, szText);
		setStringRGBA(br, bg, bb, ba);
		setStringBorderRGBA(cr, cg, cb, ca);
		graphics = tmp;
#else
		g->Clear(Color(0, 0, 0, 0));

		PointF position(0, 0);

		FontFamily ff;
		int fontStyle;
		checkFontFamily(&ff, fontStyle);
		StringFormat sf;

		GraphicsPath path;
		wchar_t* wstr = utf8_to_utf16(szText);
		path.AddString(wstr, wcslen(wstr), &ff, fontStyle, getStringSize(), position, &sf);
		delete wstr;

		float fr, fg, fb, fa;
		if (getStringBorder())
		{
			getStringBorderRGBA(fr, fg, fb, fa);
			Pen pen(Color(floatToUint8(fa),
				floatToUint8(fr),
				floatToUint8(fg),
				floatToUint8(fb)), getStringBorder());
			pen.SetLineJoin(LineJoinRound);
			g->DrawPath(&pen, &path);
		}

		getStringRGBA(fr, fg, fb, fa);
		SolidBrush brush(Color(floatToUint8(fa),
			floatToUint8(fr),
			floatToUint8(fg),
			floatToUint8(fb)));
		g->FillPath(&brush, &path);
#endif

		Rect rect(0, 0, devSize.width, devSize.height);
		BitmapData bmpData;
		bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

		int stride = bmpData.Stride / 4;
		//uint32* pixels = (uint32*)bmpData.Scan0;
		uint8* rgba = (uint8*)bmpData.Scan0;
		// 0 ====> width -1
		int left = bmpData.Width;
		for (int i = 0; i < bmpData.Width; i++)
		{
			bool exist = false;
			for (int j = 0; j < bmpData.Height; j++)
			{
				if (rgba[bmpData.Stride * j + 4 * i + 3])
				{
					exist = true;
					break;
				}
			}
			if (exist)
			{
				left = i;
				break;
			}
		}
		// 0 <==== width -1
		int right = -1;
		for (int i = bmpData.Width - 1; i > -1; i--)
		{
			bool exist = false;
			for (int j = 0; j < bmpData.Height; j++)
			{
				if (rgba[bmpData.Stride * j + 4 * i + 3])
				{
					exist = true;
					break;
				}
			}
			if (exist)
			{
				right = i;
				break;
			}
		}
		if (right < left)
		{
			bmp->UnlockBits(&bmpData);
			return iRectMake(0, 0, 0, 0);
		}

		int top;
		for (int j = 0; j < bmpData.Height; j++)
		{
			bool exist = false;
			for (int i = 0; i < bmpData.Width; i++)
			{
				if (rgba[bmpData.Stride * j + 4 * i + 3])
				{
					exist = true;
					break;
				}
			}
			if (exist)
			{
				top = j;
				break;
			}
		}
		int bottom;
		for (int j = bmpData.Height - 1; j > -1; j--)
		{
			bool exist = false;
			for (int i = 0; i < bmpData.Width; i++)
			{
				if (rgba[bmpData.Stride * j + 4 * i + 3])
				{
					exist = true;
					break;
				}
			}
			if (exist)
			{
				bottom = j;
				break;
			}
		}

		bmp->UnlockBits(&bmpData);
		if (bottom < top)
			return iRectMake(0, 0, 0, 0);

		return iRectMake(left, top, right - left + 1, bottom - top + 1);
	}

public:
	Bitmap* bmp;
	Graphics* g;
};

static StringSize* ss = NULL;

iRect iGraphics::rectOfString(const char* szFormat, ...)
{
	if (ss == NULL)
		ss = new StringSize();

	char szText[1024];
	va_start_end(szText, szFormat);

	return ss->rect(szText);
}

void iGraphics::drawString(float x, float y, int anc, const char* szFormat, ...)
{
	char szText[1024];
	va_start_end(szText, szFormat);

	iRect r = rectOfString(szText);
	x -= r.origin.x;
	y -= r.origin.y;

	switch (anc) {
	case TOP | LEFT:														break;
	case TOP | RIGHT:		x -= r.size.width;								break;
	case TOP | HCENTER:		x -= r.size.width / 2;							break;

	case BOTTOM | LEFT:								y -= r.size.height;		break;
	case BOTTOM | RIGHT:	x -= r.size.width;		y -= r.size.height;		break;
	case BOTTOM | HCENTER:	x -= r.size.width / 2;	y -= r.size.height;		break;

	case VCENTER | LEFT:							y -= r.size.height / 2;	break;
	case VCENTER | RIGHT:	x -= r.size.width;		y -= r.size.height / 2;	break;
	case VCENTER | HCENTER:	x -= r.size.width / 2;	y -= r.size.height / 2;	break;
	}
	PointF position(x, y);

	FontFamily ff;
	int fontStyle;
	checkFontFamily(&ff, fontStyle);
	StringFormat sf;

	GraphicsPath path;
	wchar_t* wstr = utf8_to_utf16(szText);
	path.AddString(wstr, wcslen(wstr), &ff, fontStyle, getStringSize(), position, &sf);
	delete wstr;

	float fr, fg, fb, fa;
	if (getStringBorder())
	{
		getStringBorderRGBA(fr, fg, fb, fa);
		Pen pen(Color(floatToUint8(fa),
			floatToUint8(fr),
			floatToUint8(fg),
			floatToUint8(fb)), getStringBorder());
		pen.SetLineJoin(LineJoinRound);
		g->DrawPath(&pen, &path);
	}

	getStringRGBA(fr, fg, fb, fa);
	SolidBrush brush(Color(floatToUint8(fa),
		floatToUint8(fr),
		floatToUint8(fg),
		floatToUint8(fb)));
	g->FillPath(&brush, &path);
}

//#define CP_ENCODE CP_ACP
#define CP_ENCODE CP_UTF8
wchar_t* utf8_to_utf16(const char* szFormat, ...)
{
	char szText[1024];
	va_start_end(szText, szFormat);

	int length = MultiByteToWideChar(CP_ENCODE, 0, szText, -1, NULL, 0);
	wchar_t* ws = new wchar_t[length];
	MultiByteToWideChar(CP_ENCODE, 0, szText, strlen(szText) + 1, ws, length);

	return ws;
}

char* utf16_to_utf8(const wchar_t* str)
{
	int length = WideCharToMultiByte(CP_ENCODE, 0, str, lstrlenW(str), NULL, 0, NULL, NULL);
	char* s = new char[length + 1];
	WideCharToMultiByte(CP_ENCODE, 0, str, lstrlenW(str), s, length, NULL, NULL);
	s[length] = 0;

	return s;
}
#endif// (OS==OS_WINDOWS)

