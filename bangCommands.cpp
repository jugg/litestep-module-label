#include "common.h"
#include "bangCommands.h"
#include "Label.h"
#include "Font.h"

extern HINSTANCE hInstance;
extern LabelList labelList;

Label *lookupLabel(const string &name);

// forwards declarations
void AlwaysOnTopBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void DestroyBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void HideBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void MoveBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void PinToDesktopBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void RepositionBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ResizeBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void SetFontColorBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void SetTextBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ShowBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ToggleAlwaysOnTopBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ToggleBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void UpdateBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ClipboardBangCommand(HWND caller, const char *bangCommandName, const char *arguments);

// bang commands implemented by this module
struct { const char *name; BangCommandEx *function; } bangCommands[] = {
	{ "!%sAlwaysOnTop", AlwaysOnTopBangCommand },
	{ "!%sDestroy", DestroyBangCommand },
	{ "!%sHide", HideBangCommand },
	{ "!%sMove", MoveBangCommand },
	{ "!%sPinToDesktop", PinToDesktopBangCommand },
	{ "!%sReposition", RepositionBangCommand },
	{ "!%sResize", ResizeBangCommand },
	{ "!%sSetFontColor", SetFontColorBangCommand },
	{ "!%sSetText", SetTextBangCommand },
	{ "!%sShow", ShowBangCommand },
	{ "!%sToggleAlwaysOnTop", ToggleAlwaysOnTopBangCommand },
	{ "!%sToggle", ToggleBangCommand },
	{ "!%sUpdate", UpdateBangCommand },
	{ "!%sClipboard", ClipboardBangCommand }
};

const int bangCommandCount = sizeof(bangCommands) / sizeof(bangCommands[0]);

// add bang commands for the label identified by 'labelName'
void AddBangCommands(const string &labelName)
{
	for(int i = 0; i < bangCommandCount; i++)
	{
		char name[64];
		wsprintf(name, bangCommands[i].name, labelName.c_str());

		AddBangCommandEx(name, bangCommands[i].function);
	}
}

// remove bang commands for a label
void RemoveBangCommands(const string &labelName)
{
	for(int i = 0; i < bangCommandCount; i++)
	{
		char name[64];
		wsprintf(name, bangCommands[i].name, labelName.c_str());

		RemoveBangCommand(name);
	}
}

// create a label
void CreateLabelBangCommand(HWND caller, const char *arguments)
{
	Label *label = new Label(arguments);
	label->load(hInstance);
	labelList.insert(labelList.end(), label);
}

//LsBox BangHook support by blkhawk
void LsBoxHookBangCommand(HWND caller, const char *arguments)
{
	char labelName[MAX_LINE_LENGTH];
	LPCTSTR nextToken[MAX_LINE_LENGTH];

	GetToken(arguments,labelName,nextToken,false);

	//check if the label is already running
	Label *label = lookupLabel(labelName);

    if (label == 0) 
	{
		label = new Label(labelName);
		label->load(hInstance);
		labelList.insert(labelList.end(), label);
	}

	char *handle = strrchr(arguments,' ');

	if ((label != 0)&&(handle))
		{
			HWND boxWnd = (HWND)atoi(handle+1);
			if (boxWnd) 
			{
				label->setBox(boxWnd);
				label->show();
				label->update();
			}
		}
}

// display label debugging info
void DebugBangCommand(HWND caller, const char *arguments)
{
	LabelListIterator it = labelList.begin();
	string message;

	while(it != labelList.end())
	{
		Label *label = *it;
		string temp = label->getName();

		if(label->getBox() != 0)
		{
			char buffer[80];
			GetWindowText(label->getBox(), buffer, 80);
			temp += " " + string(buffer);
		}

		temp += "\r\n";
		message += temp;
		it++;
	}
	
	MessageBox(caller,
		message.c_str(),
		"Label Debug Info",
		MB_SETFOREGROUND);
}

// make a label always on top
void AlwaysOnTopBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 12));

	if(label != 0) label->setAlwaysOnTop(true);
}

// destroy a label
void DestroyBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 8));

	if(label != 0)
	{
		labelList.remove(label);
		delete label;
	}
}

// hide a label
void HideBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 5));

	if(label != 0) label->hide();
}

// move a label
void MoveBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 5));

	if(label != 0)
	{
		char xCoord[16];
		char yCoord[16];

		char *tokenBuffers[] = { xCoord, yCoord };

		if(LCTokenize(arguments, tokenBuffers, 2, 0) == 2)
		{
			int x = ParseCoordinate(xCoord, 0, GetSystemMetrics(SM_CXSCREEN));
			int y = ParseCoordinate(yCoord, 0, GetSystemMetrics(SM_CYSCREEN));

			label->move(x, y);
		}
	}
}

// pin a label to the desktop (opposite of always on top)
void PinToDesktopBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 13));

	if(label != 0) label->setAlwaysOnTop(false);
}

// reposition (move and resize) a label
void RepositionBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 11));

	if(label != 0)
	{
		char xCoord[16];
		char yCoord[16];
		char xSize[16];
		char ySize[16];

		char *tokenBuffers[] = { xCoord, yCoord, xSize, ySize };

		if(LCTokenize(arguments, tokenBuffers, 4, 0) == 4)
		{
			int x = ParseCoordinate(xCoord, 0, GetSystemMetrics(SM_CXSCREEN));
			int y = ParseCoordinate(yCoord, 0, GetSystemMetrics(SM_CYSCREEN));
			int width = ParseDimension(xSize, 0, GetSystemMetrics(SM_CXSCREEN));
			int height = ParseDimension(ySize, 0, GetSystemMetrics(SM_CYSCREEN));

			label->reposition(x, y, width, height);
		}
	}
}

// resize a label
void ResizeBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 7));

	if(label != 0)
	{
		char xSize[16];
		char ySize[16];

		char *tokenBuffers[] = { xSize, ySize };

		if(LCTokenize(arguments, tokenBuffers, 2, 0) == 2)
		{
			int width = ParseDimension(xSize, 0, GetSystemMetrics(SM_CXSCREEN));
			int height = ParseDimension(ySize, 0, GetSystemMetrics(SM_CYSCREEN));

			label->resize(width, height);
		}
	}
}

static int parseInt(const char *str, int radix = 10);

// set a label's font color
void SetFontColorBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 13));

	if(label != 0 && arguments != 0)
	{
		char red[8];
		char green[8];
		char blue[8];
		char *tokenBuffers[] = { red, green, blue };
		int color;
		
		int numTokens = LCTokenize(arguments, tokenBuffers, 3, 0);
		
		if(numTokens >= 3)
		{
			color = RGB(parseInt(red), parseInt(green), parseInt(blue));
		}
		else
		{
			color = parseInt(red, 16);
			color = MAKELONG(HIWORD(color), LOWORD(color));
		}
		
		label->getFont()->setColor(color);
		label->repaint();
	}
}

// set a label's text
void SetTextBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 8));

	if(label != 0)
	{
		char text[1024];
		char *tokenBuffers[] = { text };

		if(arguments && LCTokenize(arguments, tokenBuffers, 1, 0) == 1)
			label->setText(text);
		else
			label->setText("");
	}
}

// show a label
void ShowBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 5));

	if(label != 0) label->show();
}

// toggle the always on top state of a label
void ToggleAlwaysOnTopBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 18));

	if(label != 0) label->setAlwaysOnTop(!label->isAlwaysOnTop());
}

// toggle the visibility of a label
void ToggleBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 7));

	if(label != 0) label->isVisible() ? label->hide() : label->show();
}

// update the label's contents
void UpdateBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 7));

	if(label != 0) label->update();
}

// copies the label's contents to the clipboard
void ClipboardBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 10));

	if(label != 0)
	{
		string sText = label->getText();
		if(OpenClipboard(NULL))
		{
			HGLOBAL hClipBuffer;
			char* pszBuffer;
			EmptyClipboard();
			if(arguments)
			{
				hClipBuffer = GlobalAlloc(GMEM_DDESHARE, sText.length()+strlen(arguments)+2);
				pszBuffer = (char*)GlobalLock(hClipBuffer);
				strcpy(pszBuffer, arguments);
				strcat(pszBuffer, " ");
				strcat(pszBuffer, sText.c_str());
			}
			else
			{
				hClipBuffer = GlobalAlloc(GMEM_DDESHARE, sText.length()+1);
				pszBuffer = (char*)GlobalLock(hClipBuffer);
				memset(pszBuffer, 0, 1);
				strcpy(pszBuffer, sText.c_str());
			}
			GlobalUnlock(hClipBuffer);
			SetClipboardData(CF_TEXT, hClipBuffer);
			CloseClipboard();
		}
	}
}

// value of digit char
static int digitValueOf(char ch)
{
	if(ch >= 'A' && ch <= 'F') {
		return (ch - 'A') + 10;
	}
	else if(ch >= 'a' && ch <= 'f') {
		return (ch - 'a') + 10;
	}
	else if(ch >= '0' && ch <= '9') {
		return (ch - '0');
	}
	else {
		return -1;
	}
}

// parse an int using given radix
static int parseInt(const char *str, int radix)
{
	int value = 0;
	
	while(*str)
		value = (value * radix) + digitValueOf(*str++);
	
	return value;
}
