#include "common.h"
#include "Font.h"

Font::Font()
{
	hFont = 0;
}

Font::~Font()
{
	if(hFont != 0)
		DeleteObject(hFont);
}

void Font::configure(const string &prefix)
{
	shadow = GetRCBoolean(prefix, "FontShadow");
	shadowColor = GetRCColor(prefix, "FontShadowColor", RGB(0, 0, 0));
	shadowX = GetRCInt(prefix, "FontShadowX", 1);
	shadowY = GetRCInt(prefix, "FontShadowY", 1);

	name = GetRCString(prefix, "Font", "Arial");
	height = GetRCInt(prefix, "FontHeight", 15, 0);
	color = GetRCColor(prefix, "FontColor", shadow ? RGB(255, 255, 255) : RGB(0, 0, 0));

	bold = GetRCBoolean(prefix, "FontBold");
	italic = GetRCBoolean(prefix, "FontItalic");
}

void Font::apply(HDC hDC, int x, int y, int width, int height, const string &text, unsigned int flags)
{
	if(hFont == 0)
		createHandle();

	RECT r;

	r.left = x;
	r.top = y;
	r.right = x + width;
	r.bottom = y + height;

	hFont = (HFONT) SelectObject(hDC, hFont);
	int bkMode = SetBkMode(hDC, TRANSPARENT);
	int textColor = SetTextColor(hDC, color);

	DrawTextEx(hDC, (char *) text.c_str(), text.length(), &r, flags | DT_CALCRECT, 0);

	int neededHeight = r.bottom - r.top;
	int vPad = (height - neededHeight) / 2;

	r.left = x;
	r.top = y + vPad;
	r.right = x + width;
	r.bottom = y + neededHeight + vPad;

	if(shadow)
	{
		OffsetRect(&r, (shadowX + 1) / 2, (shadowY + 1) / 2);
		SetTextColor(hDC, shadowColor);
		DrawTextEx(hDC, (char *) text.c_str(), text.length(), &r, flags, 0);
		OffsetRect(&r, -shadowX, -shadowY);
	}

	SetTextColor(hDC, color);
	DrawTextEx(hDC, (char *) text.c_str(), text.length(), &r, flags, 0);

	hFont = (HFONT) SelectObject(hDC, hFont);
	SetBkMode(hDC, bkMode);
	SetTextColor(hDC, textColor);
}

void Font::measure(HDC hDC, const string &text, unsigned int flags, int *width, int *height)
{
	// ...
}

void Font::createHandle()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	strncpy(lf.lfFaceName, name.c_str(), LF_FACESIZE);
	lf.lfHeight = height;
	lf.lfItalic = italic ? TRUE : FALSE;
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;

	if(hFont != 0)
		DeleteObject(hFont);

	hFont = CreateFontIndirect(&lf);
}
