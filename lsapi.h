#ifndef  __LSAPI_H
#define __LSAPI_H
#include <stdio.h>
#include "lsmultimon.h"
#include "wharfdata.h"
#ifdef LSAPI_INTERNAL
  #define LSAPI __declspec(dllexport)
#else
  #define LSAPI
#endif



//---------------------------------------------------------
// Defines
//---------------------------------------------------------

/* match defines */
#define MATCH_PATTERN  6    /* bad pattern */
#define MATCH_LITERAL  5    /* match failure on literal match */
#define MATCH_RANGE    4    /* match failure on [..] construct */
#define MATCH_ABORT    3    /* premature end of text string */
#define MATCH_END      2    /* premature end of pattern string */
#define MATCH_VALID    1    /* valid match */

/* pattern defines */
#define PATTERN_VALID  0    /* valid pattern */
#define PATTERN_ESC   -1    /* literal escape at end of pattern */
#define PATTERN_RANGE -2    /* malformed range in [..] construct */
#define PATTERN_CLOSE -3    /* no end bracket in [..] construct */
#define PATTERN_EMPTY -4    /* [..] contstruct is empty */

/* message defines */
#define LM_SHUTDOWN          8889
#define LM_REPAINT           8890
#define LM_BRINGTOFRONT      8891
#define LM_SAVEDATA          8892
#define LM_RESTOREDATA       8893
#define LM_POPUP             9182
#define LM_HIDEPOPUP         9183
#define LM_FIRSTDESKTOPPAINT 9184
#define LM_LSSELECT          9185
#define LM_SETTASKBARONTOP   9186
#define LM_SAVESYSTRAY       9210
#define LM_RESTORESYSTRAY    9211
#define LM_CHECKFORAPPBAR    9212
#define LM_SENDSYSTRAY       9213
#define LM_SYSTRAY           9214
// Shell Hook Messages (obsolete!)
#define LM_SHELLMESSAGE      9219
#define LM_ACCESSSTATE       9220
#define LM_ACTIVESHELLWIN    LM_ACCESSSTATE + 1
#define LM_APPCOMMANDX       LM_ACCESSSTATE + 2
#define LM_MINMAXWIN         LM_ACCESSSTATE + 3
#define LM_LANGUAGEX         LM_ACCESSSTATE + 4
#define LM_REDRAWX           LM_ACCESSSTATE + 5
#define LM_TASKMANX          LM_ACCESSSTATE + 6
#define LM_ACTIVEWIN         LM_ACCESSSTATE + 7
#define LM_ADDWINDOW         LM_ACCESSSTATE + 8
#define LM_REMOVEWINDOW      LM_ACCESSSTATE + 9 //9229
// End Shell Hook Messages
#define LM_RECYCLE           9260
#define LM_REGISTERMESSAGE   9263
#define LM_UNREGISTERMESSAGE 9264
#define LM_GETREVID          9265
#define LM_UNLOADMODULE      9266
#define LM_RELOADMODULE      9267
#define LM_SHADETOGGLE       9300
#define LM_REFRESH           9305
#define LM_THREAD_BANGCOMMAND 9310
#define LM_THREADREADY       9311
#define LM_THREADFINISHED    9312

// VWM messages
#define LM_VWMUP                   9350
#define LM_VWMDOWN                 9351
#define LM_VWMLEFT                 9352
#define LM_VWMRIGHT                9353
#define LM_VWMNAV                  9354
#define LM_SWITCHTON               9355
#define LM_ISSTICKY                9356
#define LM_STICK                   9357
#define LM_UNSTICK                 9358
#define LM_LISTDESKTOPS  9359
#define LM_DESKTOPINFO  9360
#define LM_GETDESKTOPOF  9361

// core messages
#define LM_GETLSOBJECT             9400
#define LM_WINDOWLIST              9401
#define LM_DATASTORE               9410
#define LM_MESSAGEMANAGER          9411
#define LM_BANGCOMMAND             9420

// shell hook messages
#define LM_WINDOWCREATED           9501
#define LM_WINDOWDESTROYED         9502
#define LM_ACTIVATESHELLWINDOW     9503
#define LM_WINDOWACTIVATED         9504
#define LM_GETMINRECT              9505
#define LM_REDRAW                  9506
#define LM_TASKMAN                 9507
#define LM_LANGUAGE                9508
#define LM_ACCESSIBILITYSTATE      9511
#define LM_APPCOMMAND              9512

// general constants
//

#define MAX_BANGCOMMAND            64
#define MAX_BANGARGS               256
#define MAX_RCCOMMAND              64
#define MAX_LINE_LENGTH            4096
#define MAX_PATH_LENGTH			   1024


//---------------------------------------------------------
// Bang command struct
//---------------------------------------------------------

#ifdef __cplusplus
typedef void (BangCommand)(HWND sender, LPCSTR args);
typedef void (BangCommandEx)(HWND sender, LPCSTR args, LPCSTR command);
#else
typedef void (BangCommand)(HWND sender, LPCSTR args);
typedef void (BangCommandEx)(HWND sender, LPCSTR args, LPCSTR command);
#endif

typedef struct _LMBANGCOMMANDA
{
  UINT cbSize;
  HWND hWnd;
  CHAR szCommand[MAX_BANGCOMMAND];
  CHAR szArgs[MAX_BANGARGS];
} LMBANGCOMMANDA, *PLMBANGCOMMANDA;

#define LMBANGCOMMAND    LMBANGCOMMANDA
#define PLMBANGCOMMAND   PLMBANGCOMMANDA

typedef struct LSDESKTOPINFO {
	int size;
	char name[32];
	HICON icon;
	BOOL isCurrent;
	int number;
} LSDESKTOPINFO;

//---------------------------------------------------------
// LSAPI functions
//---------------------------------------------------------

#ifdef  __cplusplus
extern "C" {
#endif  /* __cplusplus */

	LSAPI void LitestepAPIInit();
  LSAPI BOOL DLLMain(HINSTANCE, DWORD, LPVOID);

  LSAPI FILE* LCOpen (LPCTSTR szPath);
  LSAPI BOOL LCClose (FILE *f);
  LSAPI BOOL LCReadNextCommand (FILE *f, LPSTR szBuffer, DWORD dwLength);
  LSAPI BOOL LCReadNextConfig (FILE *f, LPCSTR szPrefix, LPSTR szBuffer, DWORD dwLength);
  LSAPI BOOL LCReadNextLine (FILE *f, LPSTR szBuffer, DWORD dwLength);
  LSAPI int LCTokenize (LPCSTR szString, LPSTR* lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters);

  LSAPI BOOL SetupRC(LPCTSTR szPath);
  LSAPI void CloseRC(void);

  LSAPI int GetRCInt(LPCTSTR lpKeyName, int nDefault);
  LSAPI BOOL GetRCString(LPCTSTR lpKeyName, LPSTR value, LPCTSTR defStr, int maxLen);
  LSAPI BOOL GetRCBool(LPCTSTR lpKeyName, BOOL ifFound);
  LSAPI BOOL GetRCBoolDef(LPCTSTR lpKeyName, BOOL bDefault);
  LSAPI BOOL GetRCLine( LPCTSTR, LPTSTR, UINT, LPCTSTR );
  LSAPI COLORREF GetRCColor(LPCTSTR lpKeyName, COLORREF colDef);

  LSAPI VOID SetBangManager(void *);
  LSAPI VOID ClearBangManager();
  LSAPI BOOL AddBangCommand(LPCSTR command, BangCommand f);
  LSAPI BOOL AddBangCommandEx(LPCSTR command, BangCommandEx f);
  LSAPI BOOL RemoveBangCommand(LPCSTR command);
  LSAPI BOOL ParseBangCommand (HWND caller, LPCSTR command, LPCSTR args);

  LSAPI HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor, COLORREF cTolerance, int xoffset, int yoffset);
  LSAPI HBITMAP BitmapFromIcon (HICON hIcon);
  LSAPI HBITMAP LoadLSImage(LPCSTR szFile, LPCSTR szImage);
  LSAPI HICON LoadLSIcon (LPCSTR szImage, LPCSTR szFile);
  LSAPI void GetLSBitmapSize(HBITMAP hBitmap, LPINT x, LPINT y);
  LSAPI void TransparentBltLS (HDC dc, int nXDest, int nYDest, int nWidth, int nHeight, HDC tempDC, int nXSrc, int nYSrc, COLORREF colorTransparent);

  LSAPI int CommandTokenize (LPCSTR szString, LPSTR *lpszBuffers, DWORD dwNumBuffers, LPSTR szExtraParameters);
  LSAPI void CommandParse(LPCSTR cmd, LPSTR cmdbuf, LPSTR argsbuf, DWORD dwCmdBufSize, DWORD dwArgsBufSize);

  LSAPI HINSTANCE LSExecute(HWND Owner, LPCSTR szCommand, int nShowCmd);
  LSAPI HINSTANCE LSExecuteEx(HWND Owner, LPCSTR szOperation, LPCSTR szCommand, LPCSTR szArgs, LPCSTR szDirectory, int nShowCmd);

  LSAPI HWND GetLitestepWnd(void);
  LSAPI BOOL WINAPI LSGetLitestepPath( LPTSTR, UINT );
  LSAPI BOOL WINAPI LSGetImagePath( LPTSTR, UINT );
  LSAPI void VarExpansion(LPSTR buffer, LPCSTR value);
  LSAPI BOOL GetToken(LPCSTR szString, LPSTR szToken, LPCSTR* szNextToken, BOOL useBrackets);
  LSAPI void Frame3D(HDC dc, RECT rect, COLORREF TopColor, COLORREF BottomColor, int Width);
  LSAPI void SetDesktopArea(int left, int top, int right, int bottom);

  LSAPI BOOL match (LPCSTR pattern, LPCSTR text);
  LSAPI int matche(LPCSTR pattern, LPCSTR text);
  // LSAPI BOOL is_pattern (char *pattern);
  LSAPI BOOL is_valid_pattern (LPCSTR pattern, LPINT error_type);

  LSAPI void setupVars(LPCSTR szLSPath);
  LSAPI void setupBangs(void);

  LSAPI void GetResStr(HINSTANCE hInstance, UINT uIDText, LPSTR szText, int len, LPCSTR szDefText);
  LSAPI void GetResStrEx(HINSTANCE hInstance, UINT uIDText, LPSTR szText, int len, LPCSTR szDefText, ...);

  LSAPI int LSGetSystemMetrics(int);
  LSAPI HMONITOR LSMonitorFromWindow(HWND, DWORD);
  LSAPI HMONITOR LSMonitorFromRect(LPCRECT, DWORD);
  LSAPI HMONITOR LSMonitorFromPoint(POINT, DWORD);
  LSAPI BOOL LSGetMonitorInfo(HMONITOR, LPMONITORINFO);
  LSAPI BOOL LSEnumDisplayMonitors(HDC, LPCRECT, MONITORENUMPROC, LPARAM);
  LSAPI BOOL LSEnumDisplayDevices(PVOID, DWORD, PDISPLAY_DEVICE, DWORD);

#ifdef  __cplusplus
};
#endif  /* __cplusplus */

#endif  /* __LSAPI_H */
