#include "common.h"
#include "bangCommands.h"
#include "Label.h"
#include "SystemInfo.h"
#include "LabelSettings.h"

HINSTANCE hInstance;
HWND messageHandler;
LabelList labelList;

int lsMessages[] = {
	LM_GETREVID,
	LM_REFRESH,
	0
};

#define LM_UPDATEBG (WM_USER + 1)

Label *lookupLabel(const string &name);

LRESULT WINAPI MessageHandlerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case LM_GETREVID:
		{
			UINT uLength;
			StringCchPrintf((char*)lParam, 64, "%s %s", V_NAME, V_VERSION);
			
			if (SUCCEEDED(StringCchLength((char*)lParam, 64, &uLength)))
				return uLength;

			lParam = NULL;
			return 0;
		}
		
		case LM_REFRESH:
		{
			StringList labelNames = GetRCNameList("Labels");

			// refresh the "AllLabels" configuration
			delete defaultSettings;
			defaultSettings = new LabelSettings();

			for(LabelListIterator iter = labelList.begin(); iter != labelList.end(); iter++)
			{
				if(!(*iter)->getBox())
				{
					// destroy all labels that no longer exist and that are not in a box
					for(StringListIterator it = labelNames.begin(); it != labelNames.end(); it++)
					{
						if(_stricmp((*it).c_str(), (*iter)->getName().c_str()) == 0)
							break;
					}
					if (it == labelNames.end())
					{
						labelList.remove(*iter);
						delete *iter;
						continue;
					}
				}

				// we can reconfigure all other labels, even if they are "boxed"
				(*iter)->reconfigure();
			}

			// create the rest
			for(StringListIterator it = labelNames.begin(); it != labelNames.end(); it++)
			{
				Label *label = lookupLabel(*it);
				
				if (!label) 
				{
					label = new Label(*it);
					label->load(hInstance);
					labelList.insert(labelList.end(), label);
				}
			}
			return 0;
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
		0, 0, 0, 0, 
		0,
		0,
		hInstance,
		0);

	if (!messageHandler)
		return 1;
	
	SendMessage(GetLitestepWnd(),
		LM_REGISTERMESSAGE,
		(WPARAM) messageHandler,
		(LPARAM) lsMessages);

	::hInstance = hInstance;
	defaultSettings = new LabelSettings();
	systemInfo = new SystemInfo();

	StringList labelNames = GetRCNameList("Labels");
	labelNames.merge(GetRCNameList("Label"));
//	if(labelNames.empty()) labelNames.insert(labelNames.end(), "Label");

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

	return 0;
}

extern HDC hdcDesktop;
extern HBITMAP hbmDesktop;

void quitModule(HINSTANCE hInstance)
{
	RemoveBangCommand("!LabelCreate");
	RemoveBangCommand("!LabelDebug");
	RemoveBangCommand("!LabelLsBoxHook");

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
	delete defaultSettings;

	hbmDesktop = (HBITMAP) SelectObject(hdcDesktop, hbmDesktop);
	DeleteDC(hdcDesktop);
	DeleteObject(hbmDesktop);
}

Label *lookupLabel(const string &name)
{
	for(LabelListIterator it = labelList.begin(); it != labelList.end(); it++)
	{
		if(_stricmp(name.c_str(), (*it)->getName().c_str()) == 0)
			return *it;
	}

	return 0;
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID pvReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hInstance);
	}

	return TRUE;
}
