#if !defined(__UTILITY_H)
#define __UTILITY_H

class Font;
class Texture;

const int MAX_INTEGER = 0x7FFFFFFF;
const int MIN_INTEGER = 0x80000000;

#define OS_9X   0x0001
#define OS_NT   0x0002
#define OS_95   0x0004
#define OS_98   0x0008
#define OS_ME   0x0010
#define OS_NT4  0x0020
#define OS_2K   0x0040
#define OS_XP   0x0080
#define OS_2KXP 0x0100

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
boolean IsAppWindow(const HWND &hWnd);
boolean IsOS(const int &os);
void ModifyStyle(HWND hWnd, DWORD removeStyle, DWORD addStyle);

boolean GetRCBoolean(const string &prefix, const string &baseName, boolean defaultVal = false);
int GetRCColor(const string &prefix, const string &baseName, int defaultVal);
int LblParseCoordinate(const string &aString, int defaultVal, int maxVal);
int LblGetRCCoordinate(const string &prefix, const string &baseName, int defaultVal, int maxVal);
int ParseDimension(const string &aString, int defaultVal, int maxVal);
int GetRCDimension(const string &prefix, const string &baseName, int defaultVal, int maxVal);
Font *GetRCFont(const string &prefix, Font *defaultVal = 0);
int GetRCInt(const string &prefix, const string &baseName, int defaultVal, int minVal = MIN_INTEGER, int maxVal = MAX_INTEGER);
string GetRCLine(const string &prefix, const string &baseName, const string &defaultVal);
int GetRCNamedValue(const string &prefix, const string &baseName, const NameValuePair *nameValuePairs, int defaultVal);
StringList GetRCNameList(const string &prefix);
string GetRCString(const string &prefix, const string &baseName, const string &defaultVal);
//vector<string> GetRCStringVector(const string &prefix, const string &baseName, const string &defaultVal);
Texture *GetRCTexture(const string &prefix, const string &baseName, Texture *defaultVal = 0);

void PaintDesktopEx(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, int xSrc, int ySrc, BOOL updateCache);

void TileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp);
void HorizontalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp);
void VerticalTileBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, DWORD rasterOp);
void MultiBlt(HDC hdcDest, int xDest, int yDest, int cxDest, int cyDest, HDC hdcSrc, int xSrc, int ySrc, int cxSrc, int cySrc, int cxLeft, int cyTop, int cxRight, int cyBottom, int mode, DWORD rasterOp);

LPTSTR RegQueryStringValue(HKEY hKey, LPCTSTR pszSubKey, LPCTSTR pszValueName, LPTSTR pszBuffer, int nBufferSize, LPTSTR pszDefaultValue);

vector<string> split(string str, string sep);

#endif
