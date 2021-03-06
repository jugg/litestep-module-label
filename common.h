#if !defined(__COMMON_H)
#define __COMMON_H

#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;

class Label;

typedef list<string> StringList;
typedef list<string>::iterator StringListIterator;
//typedef vector<string> StringVector;
typedef list<Label *> LabelList;
typedef list<Label *>::iterator LabelListIterator;

typedef bool boolean;
typedef unsigned __int64 largeInt;

#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
// #include "../ls-b247/lsapi/lsapi.h"
#include "lsapi.h"

// we want to use our own versions instead of the Indie LS ones
#undef GetRCCoordinate
#undef ParseCoordinate
#define GetRCCoordinate LblGetRCCoordinate
#define ParseCoordinate LblParseCoordinate

#include "utility.h"
#include "verInfo.h"
#include "strsafe.h"

#define LM_SETLABELTEXT 9600

#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
#endif

#endif
