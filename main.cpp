#include "common.h"
#include "bangCommands.h"
#include "Label.h"
#include "SystemInfo.h"

HINSTANCE hInstance;
HWND messageHandler;
LabelList labelList;
boolean initialized = false;

#define LM_UPDATEBG (WM_USER + 1)

LRESULT WINAPI MessageHandlerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case LM_GETREVID:
		{
			char revID[80];
			wsprintf(revID, "%s %s (%s)", V_NAME, V_VERSION, V_AUTHOR);
			strcpy((char *) lParam, revID);
			return strlen(revID);
		}

		case LM_UPDATEBG:
		{
			PaintDesktopEx(0, 0, 0, 0, 0, 0, 0, TRUE);

			for(LabelListIterator i = labelList.begin(); i != labelList.end(); i++)
			{
				Label *label = *i;

				if(label->getBox() == 0)
					label->repaint(true);
			}

			return 0;
		}

		case WM_DISPLAYCHANGE:
		case WM_SETTINGCHANGE:
		{
			PostMessage(hWnd, LM_UPDATEBG, 0, 0);
			return 0;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int lsMessages[] = {
	LM_GETREVID,
	0
};

int initModuleEx(HWND hParent, HINSTANCE hInstance, const char *lsPath)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_GLOBALCLASS | CS_DBLCLKS;
	wc.lpfnWndProc = Label::windowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(Label *);
	wc.hInstance = hInstance;
	wc.hbrBackground = 0;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = "LabelLS";
	wc.hIconSm = 0;

	RegisterClassEx(&wc);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_GLOBALCLASS;
	wc.lpfnWndProc = MessageHandlerProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hbrBackground = 0;
	wc.hCursor = 0;
	wc.hIcon = 0;
	wc.lpszMenuName = 0;
	wc.lpszClassName = "LabelMessageHandlerLS";
	wc.hIconSm = 0;

	RegisterClassEx(&wc);

	messageHandler = CreateWindowEx(WS_EX_TOOLWINDOW,
		"LabelMessageHandlerLS",
		0,
		WS_POPUP,
		0, 0, 0, 0, 0, 0,
		hInstance,
		0);

	SendMessage(GetLitestepWnd(),
		LM_REGISTERMESSAGE,
		(WPARAM) messageHandler,
		(LPARAM) lsMessages);

	::hInstance = hInstance;
	systemInfo = new SystemInfo();

	StringList labelNames = GetRCNameList("Labels", "");
	if(labelNames.empty()) labelNames.insert(labelNames.end(), "Label");

	for(StringListIterator it = labelNames.begin(); it != labelNames.end(); it++)
	{
		if(GetRCBoolean(*it, "LSBoxName"))
			continue;

		Label *label = new Label(*it);
		label->load(hInstance);
		labelList.insert(labelList.end(), label);
	}

	AddBangCommand("!LabelCreate", CreateLabelBangCommand);
	AddBangCommand("!LabelDebug", DebugBangCommand);
	//LsBox Support - blkhawk
	AddBangCommand("!LabelLsBoxHook", LsBoxHookBangCommand);

	initialized = true;
	return 0;
}

int initWharfModule(HWND hParent, HINSTANCE hInstance, void *pv)
{
	if(!initialized)
	{
		MessageBox(hParent,
			"Label.dll must be loaded as a LoadModule before being loaded by Lsbox.dll",
			"Label",
			MB_SETFOREGROUND);

		return 1;
	}

	if(pv == 0)
	{
		// loaded as LSBox *Module, hParent is the main box window
		char boxName[64];
		GetWindowText(hParent, boxName, 64);

		StringList labelNames = GetRCNameList("", "Labels");
		if(labelNames.empty()) labelNames.insert(labelNames.end(), "Label");

		for(StringListIterator it = labelNames.begin(); it != labelNames.end(); it++)
		{
			string dockToBox = GetRCString(*it, "LSBoxName", "");

			if(stricmp(boxName, dockToBox.c_str()) == 0)
			{
				Label *label = new Label(*it);
				label->load(hInstance, hParent);
				labelList.insert(labelList.end(), label);
			}
		}

		return 0;
	}
	else
	{
		MessageBox(hParent,
			"Label.dll cannot be loaded as a wharf module.",
			"Label",
			MB_SETFOREGROUND);

		return 1;
	}
}

extern HDC hdcDesktop;
extern HBITMAP hbmDesktop;

void quitModule(HINSTANCE hInstance)
{
	RemoveBangCommand("!LabelCreate");

	for(LabelListIterator it = labelList.begin(); it != labelList.end(); it++)
		delete *it;

	labelList.clear();

	SendMessage(GetLitestepWnd(),
		LM_UNREGISTERMESSAGE,
		(WPARAM) messageHandler,
		(LPARAM) lsMessages);

	DestroyWindow(messageHandler);

	UnregisterClass("LabelLS", hInstance);
	UnregisterClass("LabelMessageHandlerLS", hInstance);

	delete systemInfo;

	hbmDesktop = (HBITMAP) SelectObject(hdcDesktop, hbmDesktop);
	DeleteDC(hdcDesktop);
	DeleteObject(hbmDesktop);

	initialized = false;
}

void quitWharfModule(HINSTANCE hInstance)
{
	// ...
}

Label *lookupLabel(const string &name)
{
	for(LabelListIterator it = labelList.begin(); it != labelList.end(); it++)
	{
		if(stricmp(name.c_str(), (*it)->getName().c_str()) == 0)
			return *it;
	}

	return 0;
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pvReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		/* TCHAR debugFile[MAX_PATH];
		GetRCString("LabelDebugFile", debugFile, "", MAX_PATH);
		DebugSetFileName(debugFile); */

		DisableThreadLibraryCalls(hInstance);
	}

	return TRUE;
}
