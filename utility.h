#if !defined(__UTILITY_H)
#define __UTILITY_H

class Font;
class Texture;

const int MAX_INTEGER = 0x7FFFFFFF;
const int MIN_INTEGER = 0x80000000;

enum
{
	MULTIBLT_STRETCH,
	MULTIBLT_TILE,
	MULTIBLT_TILEHORIZONTAL,
	MULTIBLT_TILEVERTICAL
};

struct NameValuePair
{
	const char *name;
	int value;
};

HWND GetLitestepDesktop();
boolean IsAppWindow(HWND hWnd);
void ModifyStyle(HWND hWnd, DWORD removeStyle, DWORD addStyle);

boolean GetRCBoolean(const string &prefix, const string &baseName);
int GetRCColor(const string &prefix, const string &baseName, int defaultVal);
int GetRCCoordinate(const string &prefix, const string &baseName, int defaultVal, int wrapVal);
int GetRCDimension(const string &prefix, const string &baseName, int defaultVal, int maxVal);
Font *GetRCFont(const string &prefix, const string &baseName);
int GetRCInt(const string &prefix, const string &baseName, int defaultVal, int minVal = MIN_INTEGER, int maxVal = MAX_INTEGER);
string GetRCLine(const string &prefix, const string &baseName, const string &defaultVal);
int GetRCNamedValue(const string &prefix, const string &baseName, const NameValuePair *nameValuePairs, int defaultVal);
StringList GetRCNameList(const string &prefix, const string &baseName, const string &defaultVal);
string GetRCString(const string &prefix, const string &baseName, const string &defaultVal);
Texture *GetRCTexture(const string &prefix, const string &baseName);

void TileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp);
void HorizontalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp);
void VerticalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp);
void MultiBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, int cxLeft, int cyTop, int cxRight, int cyBottom, int mode, DWORD rasterOp);

#endif