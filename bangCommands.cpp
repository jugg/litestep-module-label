#include "common.h"
#include "bangCommands.h"
#include "Label.h"

Label *lookupLabel(const string &name);

// forwards declarations
void AlwaysOnTopBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void HideBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void MoveBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void PinToDesktopBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void SetTextBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ShowBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ToggleAlwaysOnTopBangCommand(HWND caller, const char *bangCommandName, const char *arguments);
void ToggleBangCommand(HWND caller, const char *bangCommandName, const char *arguments);

// bang commands implemented by this module
struct { const char *name; BangCommandEx *function; } bangCommands[] = {
	{ "!%sAlwaysOnTop", AlwaysOnTopBangCommand },
	{ "!%sHide", HideBangCommand },
	// { "!%sMove", MoveBangCommand },
	{ "!%sPinToDesktop", PinToDesktopBangCommand },
	{ "!%sSetText", SetTextBangCommand },
	{ "!%sShow", ShowBangCommand },
	{ "!%sToggleAlwaysOnTop", ToggleAlwaysOnTopBangCommand },
	{ "!%sToggle", ToggleBangCommand }
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

// make a label always on top
void AlwaysOnTopBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 12));

	if(label != 0) label->setAlwaysOnTop(true);
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
			int x = atoi(xCoord);
			int y = atoi(yCoord);

			if(x < 0) x = x + GetSystemMetrics(SM_CXSCREEN);
			if(y < 0) y = y + GetSystemMetrics(SM_CYSCREEN);

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

// set a label's text
void SetTextBangCommand(HWND caller, const char *bangCommandName, const char *arguments)
{
	string labelName = string(bangCommandName);
	Label *label = lookupLabel(labelName.substr(1, labelName.length() - 8));

	if(label != 0)
	{
		char text[1024];
		char *tokenBuffers[] = { text };

		if(LCTokenize(arguments, tokenBuffers, 1, 0) == 1)
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
