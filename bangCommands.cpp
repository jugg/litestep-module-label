#include "common.h"
#include "bangCommands.h"
#include "Label.h"

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
void SetTextBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ShowBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ToggleAlwaysOnTopBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ToggleBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void UpdateBangCommand(HWND caller, const char *bangCommandName, const char *arguments);

// bang commands implemented by this module
struct { const char *name; BangCommandEx *function; } bangCommands[] = {
	{ "!%sAlwaysOnTop", AlwaysOnTopBangCommand },
	{ "!%sDestroy", DestroyBangCommand },
	{ "!%sHide", HideBangCommand },
	{ "!%sMove", MoveBangCommand },
	{ "!%sPinToDesktop", PinToDesktopBangCommand },
	{ "!%sReposition", RepositionBangCommand },
	{ "!%sResize", ResizeBangCommand },
	{ "!%sSetText", SetTextBangCommand },
	{ "!%sShow", ShowBangCommand },
	{ "!%sToggleAlwaysOnTop", ToggleAlwaysOnTopBangCommand },
	{ "!%sToggle", ToggleBangCommand },
	{ "!%sUpdate", UpdateBangCommand }
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
	// labelList.insert(labelList.end(), label);
}

// display label debugging info
void DebugBangCommand(HWND caller, const char *arguments)
{
	LabelListIterator it = labelList.begin();
	string message;

	while(it != labelList.end())
	{
		string temp = (*it)->getName();

		if((*it)->getBox() != 0)
		{
			char buffer[64];
			GetWindowText((*it)->getBox(), buffer, 64);
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

// make a label always on top
void DestroyBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 8));

	if(label != 0)
	{
		// labelList.remove(label);
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
