#include "common.h"
#include "utility.h"
#include "Font.h"
#include "DefaultTexture.h"
#include "BlankTexture.h"
#include "ImageTexture.h"

// trim leading and trailing whitespace
string trim(const string &toTrim)
{
	int start = 0;
	int end = toTrim.length() - 1;
	
	while(start <= end && isspace(toTrim[start]))
		start++;
	
	while(start <= end && isspace(toTrim[end]))
		end--;
	
	return toTrim.substr(start, end - start + 1);
}

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

// check to see if a given key exists in the step.rc
int keyExists(const string &key)
{
	const char fakeValue[] = "\001";
	char buffer[4];

	GetRCString(key.c_str(), buffer, fakeValue, 4);
	return buffer[0] != '\001';
}

// retrieve a boolean value from the step.rc
boolean GetRCBoolean(const string &object, const string &subKey, boolean defaultVal)
{
	string key = object + subKey;
	return GetRCBoolDef(key.c_str(), defaultVal) ? true : false;
}

/*
boolean GetRCBoolean(const string &object, const string &subKey)
{
	string key = object + subKey;
	string allLabelsKey = allLabels + subKey;

	return GetRCBoolDef(key.c_str(),
		GetRCBoolDef(allLabelsKey.c_str(), FALSE)) ? true : false;
}
*/

// retrieve a color value from step.rc
int GetRCColor(const string &object, const string &subKey, int defaultVal)
{
	string key = object + subKey;
	return GetRCColor(key.c_str(), defaultVal);
}

/*
int GetRCColor(const string &object, const string &subKey, int defaultVal)
{
	string key = object + subKey;
	string allLabelsKey = allLabels + subKey;

	return GetRCColor(key.c_str(),
		GetRCColor(allLabelsKey.c_str(), defaultVal));
}
*/

// parse a coordinate specification
int ParseCoordinate(const string &aString, int defaultVal, int maxVal)
{
	string strVal = trim(aString);

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

// retrieve a coordinate from step.rc, accounting for negative values and centering
int GetRCCoordinate(const string &prefix, const string &baseName, int defaultVal, int maxVal)
{
	string strVal = GetRCString(prefix, baseName, "");
	return ParseCoordinate(strVal, defaultVal, maxVal);
}

// parse a dimension
int ParseDimension(const string &aString, int defaultVal, int maxVal)
{
	string strVal = trim(aString);

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

// retrieve a dimension (absolute value or percentage) from step.rc
int GetRCDimension(const string &prefix, const string &baseName, int defaultVal, int maxVal)
{
	string strVal = GetRCString(prefix, baseName, "");
	return ParseDimension(strVal, defaultVal, maxVal);
}

// retrieve a font from step.rc
Font *GetRCFont(const string &object, const string &subKey, Font *defaultVal)
{
	if(GetRCBoolean(object, subKey + "Font") || !defaultVal)
	{
		Font *font = new Font();
		font->configure(object);
		return font;
	}
	else
	{
		return defaultVal;
	}
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

		while(i < length && (isalnum(source[i]) || source[i] == '_'))
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
StringList GetRCNameList(const string &prefix, const string &baseName)
{
	string name = prefix + baseName;
	StringList resultList;
	FILE *f;

	if(f = LCOpen(0))
	{
		char lineBuffer[1024];

		while(LCReadNextCommand(f, lineBuffer, 1024))
		{
			char *buffers[1];
			char commandName[64];
			char value[1024];

			buffers[0] = commandName;
			LCTokenize(lineBuffer, buffers, 1, value);

			if(stricmp(commandName, name.c_str()) == 0)
			{
				StringList sl = ParseNameList(value);
				resultList.insert(resultList.end(), sl.begin(), sl.end());
			}
		}

		LCClose(f);
	}

	return resultList;
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
int GetRCInt(const string &object, const string &subKey, int defaultVal, int minVal, int maxVal)
{
	string key = object + subKey;
	int answer = GetRCInt(key.c_str(), defaultVal);
	return max(min(answer, maxVal), minVal);
}

/*
int GetRCInt(const string &object, const string &subKey, int defaultVal, int minVal, int maxVal)
{
	string key = object + subKey;
	string allLabelsKey = allLabels + subKey;

	int answer = GetRCInt(key.c_str(),
		GetRCInt(allLabelsKey.c_str(), defaultVal));

	return max(min(answer, maxVal), minVal);
}
*/

// retrieve a line from step.rc
string GetRCLine(const string &object, const string &subKey, const string &defaultVal)
{
	string key = object + subKey;
	char buffer[1024];
	GetRCLine(key.c_str(), buffer, 1024, defaultVal.c_str());
	return string(buffer);
}

// retrieve a line from step.rc
/* string GetRCLine(const string &prefix, const string &baseName, const string &defaultVal)
{
	string name = prefix + baseName;
	char buffer[1024];

	if(GetRCLine(name.c_str(), buffer, 1024, 0))
		return string(buffer);

	return defaultVal;
} */

// retrieve a string value from step.rc
string GetRCString(const string &object, const string &subKey, const string &defaultVal)
{
	string key = object + subKey;
	char buffer[1024];
	GetRCString(key.c_str(), buffer, defaultVal.c_str(), 1024);
	return string(buffer);
}

/*
string GetRCString(const string &object, const string &subKey, const string &defaultVal)
{
	string key = object + subKey;
	string allLabelsKey = allLabels + subKey;

	char allLabelsBuffer[1024];
	char buffer[1024];

	GetRCString(allLabelsKey.c_str(), allLabelsBuffer, defaultVal.c_str(), 1024);
	GetRCString(key.c_str(), buffer, allLabelsBuffer, 1024);

	return string(buffer);
}
*/

StringVector GetRCStringVector(const string &prefix, const string &baseName, const string &defaultVal)
{
	string name = prefix + baseName;
	StringVector result;
	FILE *f;

	if(f = LCOpen(0))
	{
		char lineBuffer[1024];

		while(LCReadNextCommand(f, lineBuffer, 1024))
		{
			char *buffers[2];
			char commandName[64];
			char value[1024] = { 0 };

			buffers[0] = commandName;
			buffers[1] = value;

			LCTokenize(lineBuffer, buffers, 2, 0);

			if(stricmp(commandName, name.c_str()) == 0)
			{
				result.push_back(string(value));
			}
		}

		LCClose(f);
	}

	if(result.empty() && !defaultVal.empty()) result.push_back(defaultVal);
	return result;
}

// retrieve a texture from step.rc
Texture *GetRCTexture(const string &object, const string &subKey, Texture *defaultVal)
{
	Texture *texture;

	if(GetRCBoolean(object, subKey + "Image"))
	{
		texture = new ImageTexture();
		texture->configure(object, subKey);
	}
	else if(GetRCBoolean(object, subKey + "Transparent"))
	{
		texture = new BlankTexture();
		texture->configure(object, subKey);
	}
	else if(defaultVal)
	{
		texture = defaultVal;
	}
	else
	{
		texture = new DefaultTexture();
		texture->configure(object, subKey);
	}

	return texture;
}

HDC hdcDesktop = 0;
HBITMAP hbmDesktop = 0;

// enhanced version of PaintDesktop
void PaintDesktopEx(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, int xSrc, int ySrc, BOOL updateCache)
{
	if(hdcDesktop == 0 || updateCache)
	{
		if(hdcDesktop == 0 && hdcDest == 0)
			return;

		if(hbmDesktop != 0)
		{
			hbmDesktop = (HBITMAP) SelectObject(hdcDesktop, hbmDesktop);
			DeleteObject(hbmDesktop);
			hbmDesktop = 0;
		}

		HDC hdcScreen = GetDC(0);

		if(hdcDesktop == 0)
		{
			hdcDesktop = CreateCompatibleDC(hdcScreen);
		}
		
		int cxDesktop = GetSystemMetrics(SM_CXSCREEN);
		int cyDesktop = GetSystemMetrics(SM_CYSCREEN);

		hbmDesktop = CreateCompatibleBitmap(hdcScreen, cxDesktop, cyDesktop);
		hbmDesktop = (HBITMAP) SelectObject(hdcDesktop, hbmDesktop);

		RECT r;
		SetRect(&r, 0, 0, cxDesktop, cyDesktop);
		FillRect(hdcDesktop, &r, GetSysColorBrush(COLOR_DESKTOP));

		char wallpaper[MAX_PATH] = { 0 };

		RegQueryStringValue(HKEY_CURRENT_USER,
			"Control Panel\\Desktop", "Wallpaper", wallpaper,
			MAX_PATH, "");

		if(wallpaper[0])
		{
			int wallpaperStyle;
			int tileWallpaper;

			char buffer[32];

			RegQueryStringValue(HKEY_CURRENT_USER, "Control Panel\\Desktop", "WallpaperStyle", buffer, 32, "0");
			wallpaperStyle = atoi(buffer);

			RegQueryStringValue(HKEY_CURRENT_USER, "Control Panel\\Desktop", "TileWallpaper", buffer, 32, "0");
			tileWallpaper = atoi(buffer);

			HBITMAP hbmWallpaper = (HBITMAP) LoadImage(0, wallpaper,
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

			if(hbmWallpaper)
			{
				BITMAP bm;
				GetObject(hbmWallpaper, sizeof(BITMAP), &bm);

				int cxWallpaper = bm.bmWidth;
				int cyWallpaper = bm.bmHeight;

				HDC hdcWallpaper = CreateCompatibleDC(hdcScreen);
				hbmWallpaper = (HBITMAP) SelectObject(hdcWallpaper, hbmWallpaper);

				if(tileWallpaper)
				{
					TileBlt(hdcDesktop,
						0, 0,
						cxDesktop, cyDesktop,
						hdcWallpaper,
						0, 0,
						cxWallpaper, cyWallpaper,
						SRCCOPY);
				}
				else
				{
					if(wallpaperStyle == 2) // stretch
					{
						SetStretchBltMode(hdcDesktop, STRETCH_DELETESCANS);

						StretchBlt(hdcDesktop,
							0, 0,
							cxDesktop, cyDesktop,
							hdcWallpaper,
							0, 0,
							cxWallpaper, cyWallpaper,
							SRCCOPY);
					}
					else
					{
						BitBlt(hdcDesktop,
						(cxDesktop - cxWallpaper) / 2,
						(cyDesktop - cyWallpaper) / 2,
						cxWallpaper, cyWallpaper,
						hdcWallpaper,
						0, 0,
						SRCCOPY);
					}
				}

				hbmWallpaper = (HBITMAP) SelectObject(hdcWallpaper, hbmWallpaper);
				DeleteDC(hdcWallpaper);

				DeleteObject(hbmWallpaper);
			}
		}

		ReleaseDC(0, hdcScreen);
	}

	if(hdcDest)
	{
		BitBlt(hdcDest, xDest, yDest, cxDest, cyDest,
			hdcDesktop, xSrc, ySrc, SRCCOPY);
	}
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

// retrieve a string value from the registry
LPTSTR RegQueryStringValue(HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValueName, LPTSTR pszBuffer, int nBufferSize, LPTSTR pszDefaultValue)
{
	DWORD dwDataType;
	DWORD dwDataSize = (DWORD) nBufferSize;
	HKEY hSubKey = NULL;
	LONG lErrorCode;
	
	if(pszSubKey)
	{
		lErrorCode = RegOpenKeyEx(hKey, pszSubKey, 0, KEY_QUERY_VALUE, &hSubKey);
		
		if(lErrorCode != ERROR_SUCCESS)
		{
			lstrcpyn(pszBuffer, pszDefaultValue, nBufferSize);
			return pszBuffer;
		}
		
		hKey = hSubKey;
	}
	
	lErrorCode = RegQueryValueEx(hKey, pszValueName, NULL, &dwDataType,
		(LPBYTE) pszBuffer, &dwDataSize);
	
	if(hSubKey)
		RegCloseKey(hSubKey);
		
	if(dwDataType != REG_SZ || lErrorCode != ERROR_SUCCESS)
		lstrcpyn(pszBuffer, pszDefaultValue, nBufferSize);
	
	return pszBuffer;
}
