#include "common.h"
#include "bangCommands.h"
#include "Font.h"
#include "Label.h"
#include "SystemInfo.h"
#include "Texture.h"

#define TIMER_MOUSETRACK 1
#define TIMER_UPDATE 2

Label::Label(const string &name)
{
	this->name = name;

	mousePressed = false;
	mouseInside = false;

	hWnd = 0;
	hInstance = 0;
	visible = false;

	backgroundDC = 0;
	bufferDC = 0;
	backgroundBitmap = 0;
	bufferBitmap = 0;

	background = 0;
	font = 0;

	AddBangCommands(name);
}

Label::~Label()
{
	RemoveBangCommands(name);

	if(hWnd != 0)
		DestroyWindow(hWnd);

	if(--instanceCount == 0)
		UnregisterClass("LabelLS", hInstance);

	if(backgroundBitmap != 0)
	{
		backgroundBitmap = (HBITMAP) SelectObject(backgroundDC, backgroundBitmap);
		DeleteDC(backgroundDC);
		DeleteObject(backgroundBitmap);
	}

	if(bufferBitmap != 0)
	{
		bufferBitmap = (HBITMAP) SelectObject(bufferDC, bufferBitmap);
		DeleteDC(bufferDC);
		DeleteObject(bufferBitmap);
	}

	delete background;
	delete font;
}

void Label::load(HINSTANCE hInstance)
{
	this->hInstance = hInstance;
	reconfigure();
}

NameValuePair justifyValues[] = {
	{ "left", DT_LEFT },
	{ "center", DT_CENTER },
	{ "right", DT_RIGHT },
	{ 0, 0 }
};

void Label::reconfigure()
{
	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	setAlwaysOnTop(GetRCBoolean(name, "AlwaysOnTop"));
	setBackground(GetRCTexture(name, ""));
	setFont(GetRCFont(name, "Font"));
	setJustify(GetRCNamedValue(name, "Justify", justifyValues, DT_CENTER));
	setText(GetRCString(name, "Text", ""));
	setUpdateInterval(GetRCInt(name, "UpdateInterval", 1000));
	setLeftBorder(GetRCInt(name, "LeftBorder", 0));
	setTopBorder(GetRCInt(name, "TopBorder", 0));
	setRightBorder(GetRCInt(name, "RightBorder", 0));
	setBottomBorder(GetRCInt(name, "BottomBorder", 0));

	reposition(GetRCCoordinate(name, "X", 0, screenX),
		GetRCCoordinate(name, "Y", 0, screenY),
		GetRCDimension(name, "Width", 64, screenX),
		GetRCDimension(name, "Height", 64, screenY));

	leftClickCommand = GetRCLine(name, "OnLeftClick", "");
	leftDoubleClickCommand = GetRCLine(name, "OnLeftDoubleClick", "");
	middleClickCommand = GetRCLine(name, "OnMiddleClick", "");
	middleDoubleClickCommand = GetRCLine(name, "OnMiddleDoubleClick", "");
	rightClickCommand = GetRCLine(name, "OnRightClick", "");
	rightDoubleClickCommand = GetRCLine(name, "OnRightDoubleClick", "");
	enterCommand = GetRCLine(name, "OnMouseEnter", "");
	leaveCommand = GetRCLine(name, "OnMouseLeave", "");

	if(!GetRCBoolean(name, "StartHidden"))
		show();
}

void Label::setAlwaysOnTop(boolean alwaysOnTop)
{
	this->alwaysOnTop = alwaysOnTop;

	if(hWnd != 0)
	{
		ModifyStyle(hWnd, WS_POPUP, WS_CHILD);
		SetParent(hWnd, alwaysOnTop ? 0 : GetLitestepDesktop());
		ModifyStyle(hWnd, WS_CHILD, WS_POPUP);

		SetWindowPos(hWnd, alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
			0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
}

void Label::setBackground(Texture *background)
{
	delete this->background;
	this->background = background;
	repaint(true);
}

void Label::setFont(Font *font)
{
	delete this->font;
	this->font = font;
	repaint();
}

void Label::setUpdateInterval(int updateInterval)
{
	this->updateInterval = updateInterval;
	if(hWnd != 0 && dynamicText) SetTimer(hWnd, TIMER_UPDATE, updateInterval, 0);
}

void Label::setJustify(int justify)
{
	this->justify = justify;
	repaint();
}

void Label::setText(const string &text)
{
	this->originalText = text;
	this->text = systemInfo->processLabelText(text, &dynamicText);

	if(hWnd != 0)
	{
		if(dynamicText)
			SetTimer(hWnd, TIMER_UPDATE, updateInterval, 0);
		else
			KillTimer(hWnd, TIMER_UPDATE);
	}

	repaint();
}

void Label::setLeftBorder(int leftBorder)
{
	this->leftBorder = leftBorder;
	repaint();
}

void Label::setTopBorder(int topBorder)
{
	this->topBorder = topBorder;
	repaint();
}

void Label::setRightBorder(int rightBorder)
{
	this->rightBorder = rightBorder;
	repaint();
}

void Label::setBottomBorder(int bottomBorder)
{
	this->bottomBorder = bottomBorder;
	repaint();
}

void Label::repaint(boolean invalidateCache)
{
	if(hWnd != 0)
	{
		if(invalidateCache)
		{
			if(backgroundDC != 0 && backgroundBitmap != 0)
			{
				backgroundBitmap = (HBITMAP) SelectObject(backgroundDC, backgroundBitmap);
				DeleteObject(backgroundBitmap);
				backgroundBitmap = 0;
			}
		}

		InvalidateRect(hWnd, 0, FALSE);
	}
}

void Label::move(int x, int y)
{
	this->x = x;
	this->y = y;

	if(hWnd != 0)
	{
		SetWindowPos(hWnd, 0, x, y, width, height,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Label::reposition(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->height = height;
	this->width = width;

	if(hWnd != 0)
	{
		SetWindowPos(hWnd, 0, x, y, width, height,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Label::resize(int width, int height)
{
	this->height = height;
	this->width = width;

	if(hWnd != 0)
	{
		SetWindowPos(hWnd, 0, x, y, width, height,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Label::hide()
{
	visible = false;

	if(hWnd != 0)
		ShowWindow(hWnd, SW_HIDE);
}

void Label::show()
{
	if(hWnd == 0)
	{
		if(++instanceCount == 1)
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
		}

		hWnd = CreateWindowEx(WS_EX_TOOLWINDOW,
			"LabelLS",
			0,
			WS_POPUP,
			x, y,
			width, height,
			0,
			0,
			hInstance,
			this);

		setAlwaysOnTop(alwaysOnTop);
		if(dynamicText) SetTimer(hWnd, TIMER_UPDATE, updateInterval, 0);
	}

	visible = true;
	ShowWindow(hWnd, SW_SHOW);
}

void Label::update()
{
	text = systemInfo->processLabelText(originalText);
	repaint();
}

void Label::onLButtonDblClk(int x, int y)
{
	if(leftDoubleClickCommand.length() > 0)
		LSExecute(hWnd, leftDoubleClickCommand.c_str(), SW_SHOWNORMAL);
}

void Label::onLButtonDown(int x, int y)
{
	if(!mousePressed)
		mousePressed = true;
}

void Label::onLButtonUp(int x, int y)
{
	if(mousePressed)
	{
		POINT pt;
		pt.x = x;
		pt.y = y;

		RECT rc;
		GetClientRect(hWnd, &rc);

		if(PtInRect(&rc, pt))
		{
			if(leftClickCommand.length() > 0)
				LSExecute(hWnd, leftClickCommand.c_str(), SW_SHOWNORMAL);
		}

		mousePressed = false;
	}
}

void Label::onMButtonDblClk(int x, int y)
{
	if(middleDoubleClickCommand.length() > 0)
		LSExecute(hWnd, middleDoubleClickCommand.c_str(), SW_SHOWNORMAL);
}

void Label::onMButtonDown(int x, int y)
{
	if(!mousePressed)
		mousePressed = true;
}

void Label::onMButtonUp(int x, int y)
{
	if(mousePressed)
	{
		POINT pt;
		pt.x = x;
		pt.y = y;

		RECT rc;
		GetClientRect(hWnd, &rc);

		if(PtInRect(&rc, pt))
		{
			if(middleClickCommand.length() > 0)
				LSExecute(hWnd, middleClickCommand.c_str(), SW_SHOWNORMAL);
		}

		mousePressed = false;
	}
}

void Label::onRButtonDblClk(int x, int y)
{
	if(rightDoubleClickCommand.length() > 0)
		LSExecute(hWnd, rightDoubleClickCommand.c_str(), SW_SHOWNORMAL);
}

void Label::onRButtonDown(int x, int y)
{
	if(!mousePressed)
		mousePressed = true;
}

void Label::onRButtonUp(int x, int y)
{
	if(mousePressed)
	{
		POINT pt;
		pt.x = x;
		pt.y = y;

		RECT rc;
		GetClientRect(hWnd, &rc);

		if(PtInRect(&rc, pt))
		{
			if(rightClickCommand.length() > 0)
				LSExecute(hWnd, rightClickCommand.c_str(), SW_SHOWNORMAL);
		}

		mousePressed = false;
	}
}

void Label::onMouseEnter()
{
	if(enterCommand.length() > 0)
		LSExecute(hWnd, enterCommand.c_str(), SW_SHOWNORMAL);
}

void Label::onMouseLeave()
{
	if(leaveCommand.length() > 0)
		LSExecute(hWnd, leaveCommand.c_str(), SW_SHOWNORMAL);
}

void Label::onMouseMove(int x, int y)
{
	if(!mouseInside)
	{
		mouseInside = true;
		onMouseEnter();
	}

	SetTimer(hWnd, TIMER_MOUSETRACK, 100, 0);
}

void Label::onPaint(HDC hDC)
{
	RECT r;
	GetWindowRect(hWnd, &r);

	int width = r.right - r.left;
	int height = r.bottom - r.top;

	// keep a cached rendition of the background
	if(backgroundBitmap == 0)
	{
		if(backgroundDC == 0)
			backgroundDC = CreateCompatibleDC(hDC);

		backgroundBitmap = CreateCompatibleBitmap(hDC, width, height);
		backgroundBitmap = (HBITMAP) SelectObject(backgroundDC, backgroundBitmap);

		if(background->isTransparent())
		{
			// paint desktop on display DC and then into background buffer
			PaintDesktopEx(backgroundDC, 0, 0, width, height, r.left, r.top, FALSE);
			// PaintDesktop(hDC);
			// BitBlt(backgroundDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
		}
		
		background->apply(backgroundDC, 0, 0, width, height);
	}

	// double buffer for flicker-free paint
	if(bufferBitmap == 0)
	{
		if(bufferDC == 0)
			bufferDC = CreateCompatibleDC(hDC);

		bufferBitmap = CreateCompatibleBitmap(hDC, width, height);
		bufferBitmap = (HBITMAP) SelectObject(bufferDC, bufferBitmap);
	}

	// blt background into double buffer
	BitBlt(bufferDC, 0, 0, width, height, backgroundDC, 0, 0, SRCCOPY);

	// render text
	font->apply(bufferDC,
		leftBorder,
		topBorder,
		width - leftBorder - rightBorder,
		height - topBorder - bottomBorder,
		text,
		justify | DT_SINGLELINE | DT_VCENTER);

	// blt the double buffer to the display
	BitBlt(hDC, 0, 0, width, height, bufferDC, 0, 0, SRCCOPY);
}

void Label::onSize(int width, int height)
{
	this->height = height;
	this->width = width;

	if(backgroundDC != 0 && backgroundBitmap != 0)
	{
		backgroundBitmap = (HBITMAP) SelectObject(backgroundDC, backgroundBitmap);
		DeleteObject(backgroundBitmap);
		backgroundBitmap = 0;
	}

	if(bufferDC != 0 && bufferBitmap != 0)
	{
		bufferBitmap = (HBITMAP) SelectObject(bufferDC, bufferBitmap);
		DeleteObject(bufferBitmap);
		bufferBitmap = 0;
	}

	InvalidateRect(hWnd, 0, FALSE);
}

void Label::onTimer(int timerID)
{
	if(timerID == TIMER_MOUSETRACK)
	{
		POINT pt;
		GetCursorPos(&pt);

		RECT rc;
		GetWindowRect(hWnd, &rc);

		if(!PtInRect(&rc, pt))
		{
			KillTimer(hWnd, TIMER_MOUSETRACK);
			mouseInside = false;
			onMouseLeave();
		}
	}
	else if(timerID == TIMER_UPDATE)
	{
		update();
	}
}

boolean Label::onWindowMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
	switch(message)
	{
		case WM_LBUTTONDBLCLK:
		{
			onLButtonDblClk((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_LBUTTONDOWN:
		{
			onLButtonDown((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_LBUTTONUP:
		{
			onLButtonUp((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_MBUTTONDBLCLK:
		{
			onMButtonDblClk((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_MBUTTONDOWN:
		{
			onMButtonDown((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_MBUTTONUP:
		{
			onMButtonUp((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_RBUTTONDBLCLK:
		{
			onRButtonDblClk((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_RBUTTONDOWN:
		{
			onRButtonDown((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_RBUTTONUP:
		{
			onRButtonUp((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_MOUSEMOVE:
		{
			onMouseMove((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC;

			if(wParam == 0)
				hDC = BeginPaint(hWnd, &ps);
			else
				hDC = (HDC) wParam;

			onPaint(hDC);

			if(wParam == 0)
				EndPaint(hWnd, &ps);

			return true;
		}

		case WM_SIZE:
		{
			onSize((int) (short) LOWORD(lParam), (int) (short) HIWORD(height));
			return true;
		}

		case WM_TIMER:
		{
			onTimer((int) wParam);
			return true;
		}
	}

	return false;
}

LRESULT Label::windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Label *label = (Label *) GetWindowLong(hWnd, 0);

	if(message == WM_NCCREATE)
	{
		label = (Label *) ((CREATESTRUCT *) lParam)->lpCreateParams;
		label->hWnd = hWnd;
		SetWindowLong(hWnd, 0, (LONG) label);
	}

	if(label)
	{
		LRESULT lResult = 0;

		if(label->onWindowMessage(message, wParam, lParam, lResult))
			return lResult;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int Label::instanceCount = 0;
