#include "common.h"
#include "SolidTexture.h"

SolidTexture::SolidTexture()
{
	backgroundColor = 0;

	bevel = false;
	darkBevelColor = 0;
	lightBevelColor = 0;
}

void SolidTexture::read(const string &prefix)
{
	backgroundColor = GetConfigColor(prefix, "Color", RGB(128, 128, 128));

	bevel = GetConfigBoolean(prefix, "Bevel");
	darkBevelColor = GetConfigColor(prefix, "DarkBevelColor", DarkenColor(backgroundColor));
	lightBevelColor = GetConfigColor(prefix, "LightBevelColor", LightenColor(backgroundColor));
}

void SolidTexture::apply(HDC hDC, int x, int y, int width, int height)
{
	RECT r;

	r.left = x;
	r.top = y;
	r.right = r.left + width;
	r.bottom = r.bottom + height;

	// fill in the background
	HBRUSH backgroundBrush = CreateSolidBrush(backgroundColor);
	FillRect(hDC, &r, backgroundBrush);
	DeleteObject(backgroundBrush);

	// draw the dark bevel
	HPEN pen = CreatePen(PS_SOLID, 1, darkBevelColor);
	HPEN tempPen = (HPEN) SelectObject(hDC, pen);

	MoveToEx(hDC, r.left, r.bottom - 1, 0);
	LineTo(hDC, r.right - 1, r.bottom - 1);
	LineTo(hDC, r.right - 1, r.top);

	SelectObject(hDC, tempPen);
	DeleteObject(pen);

	// draw the light bevel
	pen = CreatePen(PS_SOLID, 1, lightBevelColor);
	tempPen = (HPEN) SelectObject(hDC, pen);
	
	LineTo(hDC, r.left, r.top);
	LineTo(hDC, r.left, r.bottom - 1);

	SelectObject(hDC, tempPen);
	DeleteObject(pen);
}

boolean SolidTexture::isTransparent() const
{
	return false;
}
