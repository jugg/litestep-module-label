#include "common.h"
#include "Label.h"
// #include "SystemInfo.h"

typedef list<Label *> LabelList;
typedef list<Label *>::iterator LabelListIterator;

HWND messageHandler;
LabelList labelList;

LRESULT WINAPI MessageHandlerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case LM_GETREVID:
		{
			strcpy((char *) lParam, "Label 1.0 (Maduin)");
			return strlen((char *) lParam);
		}

		case LM_REFRESH:
		{
			StringList labelNames = GetRCNameList("Labels", "", "Label");

			for(LabelListIterator itA = labelList.begin(); itA != labelList.end(); itA++)
			{
				for(StringListIterator itB = labelNames.begin(); itB != labelNames.end(); itB++)
				{
					if(stricmp((*itA)->getName().c_str(), itB->c_str()) == 0)
					{
						(*itA)->reconfigure();
						labelNames.erase(itB);
						break;
					}
				}

				if(itB == labelNames.end())
				{
					delete *itA;
					labelList.erase(itA);
				}
			}

			for(StringListIterator itC = labelNames.begin(); itC != labelNames.end(); itC++)
			{
				Label *label = new Label(*itC);
				label->load((HINSTANCE) GetWindowLong(hWnd, GWL_HINSTANCE));
				labelList.insert(labelList.end(), label);
			}
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

	// systemInfo = new SystemInfo();
	StringList labelNames = GetRCNameList("Labels", "", "Label");

	for(StringListIterator it = labelNames.begin(); it != labelNames.end(); it++)
	{
		Label *label = new Label(*it);
		label->load(hInstance);
		labelList.insert(labelList.end(), label);
	}

	return 0;
}

void quitModule(HINSTANCE hInstance)
{
	for(LabelListIterator it = labelList.begin(); it != labelList.end(); it++)
		delete *it;

	labelList.clear();

	SendMessage(GetLitestepWnd(),
		LM_UNREGISTERMESSAGE,
		(WPARAM) messageHandler,
		(LPARAM) lsMessages);

	DestroyWindow(messageHandler);
	UnregisterClass("LabelMessageHandlerLS", hInstance);

	// delete systemInfo;
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
