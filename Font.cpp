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

void Font::configure(const string &prefix, Font *baseFont)
{
	shadow = GetRCBoolean(prefix, "FontShadow", baseFont ? baseFont->shadow : false);
	shadowColor = GetRCColor(prefix, "FontShadowColor", baseFont ? baseFont->shadowColor : RGB(0, 0, 0));
	shadowX = GetRCInt(prefix, "FontShadowX", baseFont ? baseFont->shadowX : 1);
	shadowY = GetRCInt(prefix, "FontShadowY", baseFont ? baseFont->shadowY : 1);

	name = GetRCString(prefix, "Font", baseFont ? baseFont->name : "Arial");
	height = GetRCInt(prefix, "FontHeight", baseFont ? baseFont->height : 15, 0);
	color = GetRCColor(prefix, "FontColor", baseFont ? baseFont->color : (shadow ? RGB(255, 255, 255) : RGB(0, 0, 0)));

	bold = GetRCBoolean(prefix, "FontBold", baseFont ? baseFont->bold : false);
	italic = GetRCBoolean(prefix, "FontItalic", baseFont ? baseFont->italic : false);
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

void Font::measure(HDC hDC, const string &text, unsigned int flags, long *width, long *height)
{
	hFont = (HFONT) SelectObject(hDC, hFont);

	LPSIZE lpsize=new SIZE;

	lpsize->cx=300;
	lpsize->cy=100;

	GetTextExtentPoint32(hDC, text.c_str(), strlen(text.c_str()), lpsize);

	hFont = (HFONT) SelectObject(hDC, hFont);

	*width=lpsize->cx;
	*height=lpsize->cy;
}

void Font::setColor(int aColor)
{
	color = aColor;
}

void Font::createHandle()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	StringCchCopy(lf.lfFaceName, LF_FACESIZE, name.c_str());
	lf.lfHeight = height;
	lf.lfItalic = italic ? TRUE : FALSE;
	lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;

	if(hFont != 0)
		DeleteObject(hFont);

	hFont = CreateFontIndirect(&lf);
}
