#include "common.h"
#include "utility.h"
#include "Font.h"
#include "DefaultTexture.h"
#include "BlankTexture.h"
#include "ImageTexture.h"

// retrieve handle of the desktop window
HWND GetLitestepDesktop()
{
	HWND hWnd = FindWindow("DesktopBackgroundClass", 0);

	if(hWnd != 0)
		return hWnd;

	return GetDesktopWindow();
}

// is the given window a toplevel application window?
boolean IsAppWindow(HWND hWnd)
{
	if(!IsWindowVisible(hWnd))
		return false;

	if(GetWindow(hWnd, GW_OWNER) != 0)
		return false;

	long exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	if(exStyle & WS_EX_APPWINDOW)
		return true;

	if(exStyle & WS_EX_TOOLWINDOW)
		return false;

	if(GetWindowLong(hWnd, GWL_USERDATA) == 0x49474541)
		return false;

	return true;
}

// modify window style
void ModifyStyle(HWND hWnd, DWORD removeStyle, DWORD addStyle)
{
	SetWindowLong(hWnd, GWL_STYLE,
		(GetWindowLong(hWnd, GWL_STYLE) & ~removeStyle) | addStyle);
}

// retrieve a boolean value from the step.rc
boolean GetRCBoolean(const string &prefix, const string &baseName)
{
	string name = prefix + baseName;
	return GetRCBool(name.c_str(), TRUE) ? true : false;
}

// retrieve a color value from step.rc
int GetRCColor(const string &prefix, const string &baseName, int defaultVal)
{
	string name = prefix + baseName;
	return GetRCColor(name.c_str(), defaultVal);
}

// retrieve a coordinate from step.rc, accounting for negative values and centering
int GetRCCoordinate(const string &prefix, const string &baseName, int defaultVal, int maxVal)
{
	string strVal = GetRCString(prefix, baseName, "");
	strVal = strVal.substr(strVal.find_first_not_of(" \t"));

	if(strVal.length() == 0)
		return defaultVal;

	boolean negative = false;
	boolean center = false;
	int i = 0;
	int value = 0;

	if(strVal[i] == '-')
	{
		negative = true;
		i++;
	}
	else if(strVal[i] == '+')
	{
		i++;
	}

	while(i < strVal.length())
	{
		if(strVal[i] >= '0' && strVal[i] <= '9')
		{
			value = (value * 10) + (strVal[i] - '0');
		}
		else
		{
			if(strVal[i] == 'c' || strVal[i] == 'C')
				center = true;

			break;
		}

		i++;
	}

	if(center)
	{
		if(negative)
			value = (maxVal / 2) - value;
		else
			value = (maxVal / 2) + value;
	}
	else if(negative)
	{
		value = maxVal - value;
	}

	return value;
}

// retrieve a dimension (absolute value or percentage) from step.rc
int GetRCDimension(const string &prefix, const string &baseName, int defaultVal, int maxVal)
{
	string strVal = GetRCString(prefix, baseName, "");
	strVal = strVal.substr(strVal.find_first_not_of(" \t"));

	if(strVal.length() == 0)
		return defaultVal;

	boolean negative = false;
	boolean percentage = false;
	int i = 0;
	int value = 0;

	if(strVal[i] == '-')
	{
		negative = true;
		i++;
	}

	while(i < strVal.length())
	{
		if(strVal[i] >= '0' && strVal[i] <= '9')
		{
			value = (value * 10) + (strVal[i] - '0');
		}
		else
		{
			if(strVal[i] == '%')
				percentage = true;

			break;
		}

		i++;
	}

	if(percentage)
		value = (value * maxVal) / 100;

	if(negative)
		value = maxVal - value;

	return value;
}

// retrieve a font from step.rc
Font *GetRCFont(const string &prefix, const string &baseName)
{
	Font *font = new Font();
	font->configure(prefix + baseName);
	return font;
}

// parse a string into a list of names
StringList ParseNameList(const string &source)
{
	StringList nameList;
	int i = 0;
	int length = source.length();

	while(i < length)
	{
		string name;
		i = source.find_first_not_of(" \t", i);

		while(i < length && isalnum(source[i]))
		{
			name.append(1, source[i]);
			i++;
		}

		if(name.length() > 0)
			nameList.push_back(name);

		i = source.find_first_not_of(" \t", i);

		if(source[i] == ',')
		{
			i++;
			continue;
		}
		else
		{
			break;
		}
	}

	return nameList;
}

// retrieve a list of names from step.rc
StringList GetRCNameList(const string &prefix, const string &baseName, const string &defaultVal)
{
	return ParseNameList(GetRCLine(prefix, baseName, defaultVal));
}

// retrieve a named value from step.rc
int GetRCNamedValue(const string &prefix, const string &baseName, const NameValuePair *nameValuePairs, int defaultVal)
{
	string name = GetRCString(prefix, baseName, "");
	const NameValuePair *current = nameValuePairs;

	if(name.length() > 0)
	{
		while(current->name)
		{
			if(stricmp(current->name, name.c_str()) == 0)
				return current->value;

			current++;
		}
	}

	return defaultVal;
}

// retrieve an integer value from step.rc, make sure it is between minVal and maxVal inclusive
int GetRCInt(const string &prefix, const string &baseName, int defaultVal, int minVal, int maxVal)
{
	string name = prefix + baseName;
	return max(min(GetRCInt(name.c_str(), defaultVal), maxVal), minVal);
}

// retrieve a line from step.rc
string GetRCLine(const string &prefix, const string &baseName, const string &defaultVal)
{
	string name = prefix + baseName;
	char buffer[1024];

	if(GetRCLine(name.c_str(), buffer, 1024, 0))
		return string(buffer);

	return defaultVal;
}

// retrieve a string value from step.rc
string GetRCString(const string &prefix, const string &baseName, const string &defaultVal)
{
	string name = prefix + baseName;
	char buffer[1024];

	if(GetRCString(name.c_str(), buffer, 0, 1024))
		return string(buffer);

	return defaultVal;
}

// retrieve a texture from step.rc
Texture *GetRCTexture(const string &prefix, const string &baseName)
{
	string name = prefix + baseName;
	Texture *texture;

	if(GetRCBoolean(name, "Image"))
	{
		texture = new ImageTexture();
		texture->configure(name);
	}
	else if(GetRCBoolean(name, "Transparent"))
	{
		texture = new BlankTexture();
		texture->configure(name);
	}
	else
	{
		texture = new DefaultTexture();
		texture->configure(name);
	}

	return texture;
}

// tile an image horizontally and vertically
void TileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp)
{
	for(int y = 0; y < cyDest; y += cySrc)
	{
		for(int x = 0; x < cxDest; x += cxSrc)
		{
			BitBlt(hdcDest,
				xDest + x,
				yDest + y,
				min(cxDest - x, cxSrc),
				min(cyDest - y, cySrc),
				hdcSrc,
				xSrc,
				ySrc,
				rasterOp);
		}
	}
}

// tile an image horizontally
void HorizontalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp)
{
	for(int x = 0; x < cxDest; x += cxSrc)
	{
		StretchBlt(hdcDest,
			xDest + x,
			yDest,
			min(cxDest - x, cxSrc),
			cyDest,
			hdcSrc,
			xSrc,
			ySrc,
			min(cxDest - x, cxSrc),
			cySrc,
			rasterOp);
	}
}

// tile an image vertically
void VerticalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp)
{
	for(int y = 0; y < cyDest; y += cySrc)
	{
		StretchBlt(hdcDest,
			xDest,
			yDest + y,
			cxDest,
			min(cyDest - y, cySrc),
			hdcSrc,
			xSrc,
			ySrc,
			cxSrc,
			min(cyDest - y, cySrc),
			rasterOp);
	}
}

// multi-blt!
void MultiBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, int cxLeft, int cyTop, int cxRight, int cyBottom, int mode, DWORD rasterOp)
{
	if(cyTop > 0)
	{
		if(cxLeft > 0)
		{
			BitBlt(hdcDest,
				xDest,
				yDest,
				cxLeft,
				cyTop,
				hdcSrc,
				xSrc,
				ySrc,
				rasterOp);
		}

		if(mode == MULTIBLT_STRETCH || mode == MULTIBLT_TILEVERTICAL)
		{
			StretchBlt(hdcDest,
				xDest + cxLeft,
				yDest,
				cxDest - cxLeft - cxRight,
				cyTop,
				hdcSrc,
				xSrc + cxLeft,
				ySrc,
				cxSrc - cxLeft - cxRight,
				cyTop,
				rasterOp);
		}
		else
		{
			HorizontalTileBlt(hdcDest,
				xDest + cxLeft,
				yDest,
				cxDest - cxLeft - cxRight,
				cyTop,
				hdcSrc,
				xSrc + cxLeft,
				ySrc,
				cxSrc - cxLeft - cxRight,
				cyTop,
				rasterOp);
		}

		if(cxRight > 0)
		{
			BitBlt(hdcDest,
				cxDest - cxRight,
				yDest,
				cxRight,
				cyTop,
				hdcSrc,
				cxSrc - cxRight,
				ySrc,
				rasterOp);
		}
	}

	if(cxLeft > 0)
	{
		if(mode == MULTIBLT_STRETCH || mode == MULTIBLT_TILEHORIZONTAL)
		{
			StretchBlt(hdcDest,
				xDest,
				yDest + cyTop,
				cxLeft,
				cyDest - cyTop - cyBottom,
				hdcSrc,
				xSrc,
				ySrc + cyTop,
				cxLeft,
				cySrc - cyTop - cyBottom,
				rasterOp);
		}
		else
		{
			VerticalTileBlt(hdcDest,
				xDest,
				yDest + cyTop,
				cxLeft,
				cyDest - cyTop - cyBottom,
				hdcSrc,
				xSrc,
				ySrc + cyTop,
				cxLeft,
				cySrc - cyTop - cyBottom,
				rasterOp);
		}
	}

	if(mode == MULTIBLT_STRETCH)
	{
		StretchBlt(hdcDest,
			xDest + cxLeft,
			yDest + cyTop,
			cxDest - cxLeft - cxRight,
			cyDest - cyTop - cyBottom,
			hdcSrc,
			xSrc + cxLeft,
			ySrc + cyTop,
			cxSrc - cxLeft - cxRight,
			cySrc - cyTop - cyBottom,
			rasterOp);
	}
	else if(mode == MULTIBLT_TILE)
	{
		TileBlt(hdcDest,
			xDest + cxLeft,
			yDest + cyTop,
			cxDest - cxLeft - cxRight,
			cyDest - cyTop - cyBottom,
			hdcSrc,
			xSrc + cxLeft,
			ySrc + cyTop,
			cxSrc - cxLeft - cxRight,
			cySrc - cyTop - cyBottom,
			rasterOp);
	}
	else if(mode == MULTIBLT_TILEHORIZONTAL)
	{
		HorizontalTileBlt(hdcDest,
			xDest + cxLeft,
			yDest + cyTop,
			cxDest - cxLeft - cxRight,
			cyDest - cyTop - cyBottom,
			hdcSrc,
			xSrc + cxLeft,
			ySrc + cyTop,
			cxSrc - cxLeft - cxRight,
			cySrc - cyTop - cyBottom,
			rasterOp);
	}
	else if(mode == MULTIBLT_TILEVERTICAL)
	{
		VerticalTileBlt(hdcDest,
			xDest + cxLeft,
			yDest + cyTop,
			cxDest - cxLeft - cxRight,
			cyDest - cyTop - cyBottom,
			hdcSrc,
			xSrc + cxLeft,
			ySrc + cyTop,
			cxSrc - cxLeft - cxRight,
			cySrc - cyTop - cyBottom,
			rasterOp);
	}

	if(cxRight > 0)
	{
		if(mode == MULTIBLT_STRETCH || mode == MULTIBLT_TILEHORIZONTAL)
		{
			StretchBlt(hdcDest,
				cxDest - cxRight,
				yDest + cyTop,
				cxRight,
				cyDest - cyTop - cyBottom,
				hdcSrc,
				cxSrc - cxRight,
				ySrc + cyTop,
				cxRight,
				cySrc - cyTop - cyBottom,
				rasterOp);
		}
		else
		{
			VerticalTileBlt(hdcDest,
				cxDest - cxRight,
				yDest + cyTop,
				cxRight,
				cyDest - cyTop - cyBottom,
				hdcSrc,
				cxSrc - cxRight,
				ySrc + cyTop,
				cxRight,
				cySrc - cyTop - cyBottom,
				rasterOp);
		}
	}

	if(cyBottom > 0)
	{
		if(cxLeft > 0)
		{
			BitBlt(hdcDest,
				xDest,
				cyDest - cyBottom,
				cxLeft,
				cyBottom,
				hdcSrc,
				xSrc,
				cySrc - cyBottom,
				rasterOp);
		}

		if(mode == MULTIBLT_STRETCH || mode == MULTIBLT_TILEVERTICAL)
		{
			StretchBlt(hdcDest,
				xDest + cxLeft,
				cyDest - cyBottom,
				cxDest - cxLeft - cxRight,
				cyBottom,
				hdcSrc,
				xSrc + cxLeft,
				cySrc - cyBottom,
				cxSrc - cxLeft - cxRight,
				cyBottom,
				rasterOp);
		}
		else
		{
			HorizontalTileBlt(hdcDest,
				xDest + cxLeft,
				cyDest - cyBottom,
				cxDest - cxLeft - cxRight,
				cyBottom,
				hdcSrc,
				xSrc + cxLeft,
				cySrc - cyBottom,
				cxSrc - cxLeft - cxRight,
				cyBottom,
				rasterOp);
		}

		if(cxRight > 0)
		{
			BitBlt(hdcDest,
				cxDest - cxRight,
				cyDest - cyBottom,
				cxRight,
				cyBottom,
				hdcSrc,
				cxSrc - cxRight,
				cySrc - cyBottom,
				rasterOp);
		}
	}
}
