#if !defined(__COMMON_H)
#define __COMMON_H

#include <fstream>
#include <string>
#include <list>
#include <vector>

using namespace std;

class Label;

typedef list<string> StringList;
typedef list<string>::iterator StringListIterator;
typedef vector<string> StringVector;
typedef list<Label *> LabelList;
typedef list<Label *>::iterator LabelListIterator;

typedef bool boolean;
typedef __int64 largeInt;

#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>
#include <winsock.h>
#include "lsapi.h"
#include "wharfdata.h"
#include "utility.h"

#endif
