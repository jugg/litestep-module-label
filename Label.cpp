#include "common.h"
#include "bangCommands.h"
#include "LabelSettings.h"
#include "Font.h"
#include "Label.h"
#include "SystemInfo.h"
#include "Texture.h"
#include <shellapi.h>

#define TIMER_MOUSETRACK 1
#define TIMER_UPDATE 2
#define TIMER_SCROLL 3

extern LabelList labelList;

Label::Label(const string &name)
:height(0),width(0),x(0),y(0)
{
	this->name = name;

	mousePressed = false;
	mouseInside = false;

	hWnd = 0;
	hInstance = 0;
	box = 0;

	visible = false;
	bUsingDefSkin = false;
	bInDestructor = false;

	scroll = false;
	scrollLimit = 0;
	scrollPosition = 0;
	
	backgroundDC = 0;
	bufferDC = 0;
	backgroundBitmap = 0;
	bufferBitmap = 0;

	background = 0;
	font = 0;
}

Label::~Label()
{
	bInDestructor = true;

	RemoveBangCommands(name, bangCommands);

	if(IsWindow(hWnd))
	{
		// just in case...
		KillTimer(hWnd, TIMER_MOUSETRACK);
		KillTimer(hWnd, TIMER_UPDATE);
		KillTimer(hWnd, TIMER_SCROLL);
		DestroyWindow(hWnd);
	}

	if(backgroundBitmap)
	{
		backgroundBitmap = (HBITMAP) SelectObject(backgroundDC, backgroundBitmap);
		DeleteObject(backgroundBitmap);
	}
	if (backgroundDC)
		DeleteDC(backgroundDC);

	if(bufferBitmap)
	{
		bufferBitmap = (HBITMAP) SelectObject(bufferDC, bufferBitmap);
		DeleteObject(bufferBitmap);
	}
	if (bufferDC)
		DeleteDC(bufferDC);

	if(!bUsingDefSkin) delete background;
	if(font != defaultSettings->font) delete font;
}

void Label::load(HINSTANCE hInstance, HWND box)
{
	this->hInstance = hInstance;
	
    if (IsWindow(box))
    {
        this->box = box;
    }
    else
    {
        this->box = NULL;
    }
	
	if (IsWindow(hWnd))	// window already exists...
		return;

	hWnd = CreateWindowEx(box ? 0 : WS_EX_TOOLWINDOW,
		"LabelLS",
		name.c_str(),
		box ? WS_CHILD : WS_POPUP,
		x, y,			// FIXME they're all 0, the window will be repositioned
		width, height,	// FIXME see above
		box ? box : GetLitestepWnd(), // GetLitestepDesktop(), FIXME: this way there aren't as many z-order problems =)
		0,
		hInstance,
		this);
		
	if (hWnd)
	{
		SetWindowLong(hWnd, GWL_USERDATA, magicDWord);
		
		reconfigure();

		repaint();
		
		AddBangCommands(name, bangCommands);
	}
	else
		delete this;
}

void Label::reconfigure()
{
	LabelSettings settings(name.c_str());

	setAlwaysOnTop(settings.alwaysOnTop);	
	reposition(settings.x, settings.y, settings.width, settings.height);	// FIXME? doesn't check old == new

	bUseFahrenheit = settings.bUseFahrenheit;
	bangCommands = settings.bangCommands;
	
	if(!box && (bangCommands == 5))
		bangCommands = 6;				// hack for lsbox

	setBackground(settings.skin);		// FIXME: this does not check if old == new on !refresh
	setFont(settings.font);				// FIXME: see above
	setJustify(settings.justify);
	setText(settings.text);

	setUpdateInterval(settings.updateInterval);

	setLeftBorder(settings.leftBorder);
	setTopBorder(settings.topBorder);
	setRightBorder(settings.rightBorder);
	setBottomBorder(settings.bottomBorder);

	leftClickCommand = settings.leftClickCommand;
	leftDoubleClickCommand = settings.leftDoubleClickCommand;
	middleClickCommand = settings.middleClickCommand;
	middleDoubleClickCommand = settings.middleDoubleClickCommand;
	rightClickCommand = settings.rightClickCommand;
	rightDoubleClickCommand = settings.rightDoubleClickCommand;
	wheelDownCommand = settings.wheelDownCommand;
	wheelUpCommand = settings.wheelUpCommand;
	enterCommand = settings.enterCommand;
	leaveCommand = settings.leaveCommand;
	dropCommand = settings.dropCommand;

	scrollPadLength = settings.scrollPadLength;
	scrollInterval = settings.scrollInterval;
	scrollSpeed = settings.scrollSpeed;
	setScrolling(settings.scroll);

	shadowY = font->shadowY;

//	trueTransparency = settings.trueTransparency;

	if (hWnd)
	{
		DragAcceptFiles(hWnd, !dropCommand.empty());
		settings.startHidden ? hide() : show();
	}

	// FIXME
/*	if(background->isTransparent() && trueTransparency)
	{
		//set the window region that pink areas aren't part of it
		HDC tmpDC=CreateCompatibleDC(NULL);
		HBITMAP tmpBmp = CreateBitmap(width, height, 1, 32, NULL);
		HGDIOBJ tmpObj = SelectObject(tmpDC, tmpBmp);
		background->apply(tmpDC, 0, 0, width, height);
		SelectObject(tmpDC, tmpObj);

		// region handling a la MickeM =)
		HRGN region;
		region = BitmapToRegion(tmpBmp, RGB(255, 0, 255), 0, 0, 0);
		HRGN windowRgn = CreateRectRgn(0, 0, 0, 0);
		CombineRgn(windowRgn, region, NULL, RGN_COPY);

		if (!SetWindowRgn(hWnd, windowRgn, TRUE)) {
			::DeleteObject(windowRgn);
		}

		//SetWindowRgn(hWnd, BitmapToRegion(tmpBmp, RGB(255, 0, 255), 0, 0, 0), false);
		DeleteObject(tmpBmp);
		DeleteObject(tmpObj);
		DeleteDC(tmpDC);
	}*/
}

void Label::setAlwaysOnTop(boolean alwaysOnTop)
{
	this->alwaysOnTop = alwaysOnTop;
	
	if (box)
		return;
	
	if(hWnd)
	{
		ModifyStyle(hWnd, WS_POPUP, WS_CHILD);
		SetParent(hWnd, alwaysOnTop ? 0 : GetLitestepDesktop());
		ModifyStyle(hWnd, WS_CHILD, WS_POPUP);
		
		SetWindowPos(hWnd, alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
			0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
}

void Label::setBox(HWND newparent)
{
	if (this->box == newparent)
		return;

	if (this->alwaysOnTop) this->setAlwaysOnTop(FALSE);
	
	this->box = newparent;
	
	ModifyStyle(hWnd, WS_POPUP, WS_CHILD);
	SetParent(hWnd, newparent);
//	ModifyStyle(hWnd, WS_CHILD, WS_POPUP);
}

void Label::setScrolling(boolean scrolling)
{
	if (scrolling && !this->scroll)
	{
		scrollPosition = 0;
		SetTimer(hWnd, TIMER_SCROLL, scrollInterval, 0);
	}
	else if (!scrolling && this->scroll)
	{
		KillTimer(hWnd, TIMER_SCROLL);
		scrollPosition=0;
		repaint();
	}

	scrollLimit = 0;
	this->scroll = scrolling;
}

void Label::setScrollLimit(int limit)
{
	if (limit >= 0)
	{
		setScrolling(true);
		scrollLimit = limit + 1;
	}
	else
		setScrolling(false);

}

void Label::setBackground(Texture *background)
{
	if(!bUsingDefSkin)
		delete this->background;

	this->background = background;

	bUsingDefSkin = (background == defaultSettings->skin);
	
	repaint(true);
}

void Label::setFont(Font *font)
{
	if(this->font != defaultSettings->font)
		delete this->font;

	this->font = font;
	repaint();
}

void Label::setUpdateInterval(int updateInterval)
{
	this->updateInterval = updateInterval;
	if(hWnd && dynamicText) SetTimer(hWnd, TIMER_UPDATE, updateInterval, 0);
}

void Label::setJustify(int justify)
{
	if (this->justify == justify)
		return;

	this->justify = justify;
	repaint();
}

void Label::setText(const string &text)
{
	if ((this->text).compare(text) == 0)
		return;

	this->originalText = text;
	this->text = systemInfo->processLabelText(text, this, &dynamicText);

	if(hWnd)
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
	if (this->leftBorder == leftBorder)
		return;

	this->leftBorder = leftBorder;
	repaint();
}

void Label::setTopBorder(int topBorder)
{
	if (this->topBorder == topBorder)
		return;

	this->topBorder = topBorder;
	repaint();
}

void Label::setRightBorder(int rightBorder)
{
	if (this->rightBorder == rightBorder)
		return;

	this->rightBorder = rightBorder;
	repaint();
}

void Label::setBottomBorder(int bottomBorder)
{
	if (this->bottomBorder == bottomBorder)
		return;

	this->bottomBorder = bottomBorder;
	repaint();
}

void Label::repaint(boolean invalidateCache)
{
	if(hWnd)
	{
		if(invalidateCache)
		{
			if(backgroundDC && backgroundBitmap)
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

	if(hWnd)
	{
		SetWindowPos(hWnd, 0, x, y, 0, 0,
			SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Label::reposition(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->height = height;
	this->width = width;

	if(hWnd)
	{
		SetWindowPos(hWnd, 0, x, y, width, height,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Label::resize(int width, int height)
{
	this->height = height;
	this->width = width;

	if(hWnd)
	{
		SetWindowPos(hWnd, 0, 0, 0, width, height,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Label::hide()
{
	if(hWnd)
	{
		visible = false;
		ShowWindow(hWnd, SW_HIDE);
	}
}

void Label::show()
{
	if (!hWnd)
		load(hInstance, box);
	
	if(hWnd)
	{
		visible = true;
		ShowWindow(hWnd, SW_SHOWNOACTIVATE);
	}
}

void Label::update()
{
	text = systemInfo->processLabelText(originalText, this);
	repaint();
}

void Label::relayMouseMessageToBox(UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	MapWindowPoints(hWnd, box, &pt, 1);
	PostMessage(box, message, wParam, MAKELPARAM((short) pt.x, (short) pt.y));
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

void Label::onWheelDown(int x, int y)
{
	if(wheelDownCommand.length() > 0)
		LSExecute(hWnd, wheelDownCommand.c_str(), SW_SHOWNORMAL);
}

void Label::onWheelUp(int x, int y)
{
	if(wheelUpCommand.length() > 0)
		LSExecute(hWnd, wheelUpCommand.c_str(), SW_SHOWNORMAL);
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

void Label::onDrop(const string &file)
{
	// TODO: this should make a bit more sense... and should be more flexible
	string fullCommand = dropCommand + " \"" + file + "\""; // FIXME: ugly hack
	LSExecute(hWnd, fullCommand.c_str(), SW_SHOWNORMAL);
}

void Label::onPaint(HDC hDC)
{
	RECT r;
	GetWindowRect(hWnd, &r);

	int width = r.right - r.left;
	int height = r.bottom - r.top;

	// keep a cached rendition of the background
	if(!backgroundBitmap)
	{
		if(!backgroundDC)
			backgroundDC = CreateCompatibleDC(hDC);

		backgroundBitmap = CreateCompatibleBitmap(hDC, width, height);
		backgroundBitmap = (HBITMAP) SelectObject(backgroundDC, backgroundBitmap);

		if(background->isTransparent())
		{
			if(box)
			{
				// save the previous DC contents as the background
				BitBlt(backgroundDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
			}
			else
			{
				// paint desktop on display DC and then into background buffer
				PaintDesktopEx(backgroundDC, 0, 0, width, height, r.left, r.top, FALSE);
				// PaintDesktop(hDC);
				// BitBlt(backgroundDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
			}
		}
		
		background->apply(backgroundDC, 0, 0, width, height);
	}

	// double buffer for flicker-free paint
	if(!bufferBitmap)
	{
		if(!bufferDC)
			bufferDC = CreateCompatibleDC(hDC);

		bufferBitmap = CreateCompatibleBitmap(hDC, width, height);
		bufferBitmap = (HBITMAP) SelectObject(bufferDC, bufferBitmap);
	}

	// blt background into double buffer
	BitBlt(bufferDC, 0, 0, width, height, backgroundDC, 0, 0, SRCCOPY);

	// render text
	long textWidth;
	long textHeight;

	font->measure(bufferDC, text, 0, &textWidth, &textHeight);

	//don't scroll if text fits into label
	if(!(scroll && textWidth > width - leftBorder - rightBorder))
	{
		font->apply(bufferDC,
			leftBorder,
			topBorder,
			width - leftBorder - rightBorder,
			height - topBorder - bottomBorder,
			text,
			justify /* | DT_SINGLELINE */ | DT_VCENTER | DT_NOPREFIX);
	}
	else
	{
		if(scrollPosition <= 0)
		{
			scrollPosition = textWidth + scrollPadLength;
			
			if (scrollLimit)
			{
				scrollLimit--;
				if (scrollLimit == 0)
					setScrolling(false);
			}
		}

		if(scrollPosition > textWidth + scrollPadLength )
		{
			scrollPosition = 0;
		}

		HRGN oldRGN = NULL;
		HRGN newRGN = NULL;
		GetClipRgn(bufferDC, oldRGN);
		// positive shadows
		if (shadowY >= 0 && (shadowY-bottomBorder)<0 )
		{
			newRGN=CreateRectRgn(leftBorder, topBorder, width - rightBorder, height - bottomBorder + shadowY); // V
		}
		else if (shadowY >= 0)
		{
			newRGN=CreateRectRgn(leftBorder, topBorder, width - rightBorder, height - bottomBorder); // V
		}
		// handle negative shadows
		else if (shadowY<0 && topBorder+shadowY >= 0)
		{
			newRGN=CreateRectRgn(leftBorder, topBorder + shadowY, width - rightBorder, height - bottomBorder); // V
		}
		else
		{
			newRGN=CreateRectRgn(leftBorder, 0, width - rightBorder, height - bottomBorder); // V
		}
		
		SelectClipRgn(bufferDC, newRGN);
		DeleteObject(newRGN);

		font->apply(bufferDC,
			leftBorder, //(width - textWidth) / 2 - 2,
			topBorder,
			scrollPosition - scrollPadLength,
			height - topBorder - bottomBorder,
			text,
			DT_RIGHT | DT_VCENTER | DT_NOPREFIX);

		font->apply(bufferDC,
			scrollPosition + leftBorder,
			topBorder,
			textWidth,
			height - topBorder - bottomBorder,
			text,
			DT_LEFT | DT_VCENTER | DT_NOPREFIX);

		SelectClipRgn(bufferDC, oldRGN);
		DeleteObject(oldRGN);
	};


	// blt the double buffer to the display
	BitBlt(hDC, 0, 0, width, height, bufferDC, 0, 0, SRCCOPY);
}

void Label::onSize(int width, int height)
{
	this->height = height;
	this->width = width;

	if(backgroundDC && backgroundBitmap)
	{
		backgroundBitmap = (HBITMAP) SelectObject(backgroundDC, backgroundBitmap);
		DeleteObject(backgroundBitmap);
		backgroundBitmap = 0;
	}

	if(bufferDC && bufferBitmap)
	{
		bufferBitmap = (HBITMAP) SelectObject(bufferDC, bufferBitmap);
		DeleteObject(bufferBitmap);
		bufferBitmap = 0;
	}

	InvalidateRect(hWnd, 0, FALSE);
}

void Label::onTimer(int timerID)
{
	switch(timerID)
	{
		case TIMER_MOUSETRACK:

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

		break;

		case TIMER_UPDATE:

			update();

		break;

		case TIMER_SCROLL:

			scrollPosition -= scrollSpeed;
			repaint();

		break;
	}
}

void Label::onWindowPosChanged(WINDOWPOS *windowPos)
{
	repaint(true);
}

boolean Label::onWindowMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
	extern HWND messageHandler;
	
	switch(message)
	{
		case LM_SETLABELTEXT:
		{
			setText(string((const char *) lParam));
			return true;
		}

		case WM_COPYDATA:
		{
			COPYDATASTRUCT *cds = (COPYDATASTRUCT *) lParam;
			
			if(cds->dwData == LM_SETLABELTEXT)
				setText(string((const char *) cds->lpData));

			return false;
		}

		case WM_CLOSE:
		{
			lResult = 0;
			return true;
		}

		case WM_DESTROY:
		{
			if(box)		// FIXME: this is the old workaround
			{
				box = NULL;
				hide();
			}
				
			hWnd = NULL;	// FIXME: dirty hack
			
			return false;
		}

		case WM_DROPFILES:
		{
			if(dropCommand.empty())	// we shouldn't get here... but somehow we did :p
				break;
			
			int numDropped, i;
			char szFile[MAX_PATH_LENGTH];
			
			numDropped = DragQueryFile((HDROP) wParam, 0xFFFFFFFF, NULL, 0);
			for (i = 0; i < numDropped; i++)
			{
				DragQueryFile((HDROP) wParam, i, (char *)&szFile, MAX_PATH_LENGTH);
				if (szFile && szFile[0])
				{
					onDrop(szFile);
				}
			}
			DragFinish((HDROP) wParam);
			
			return 0;
		}
		
		case WM_LBUTTONDBLCLK:
		{
			if(leftDoubleClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onLButtonDblClk((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_LBUTTONDOWN:
		{
			if(leftDoubleClickCommand.empty() && leftClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onLButtonDown((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_LBUTTONUP:
		{
			if(leftDoubleClickCommand.empty() && leftClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onLButtonUp((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_MBUTTONDBLCLK:
		{
			if(middleDoubleClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onMButtonDblClk((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_MBUTTONDOWN:
		{
			if(middleDoubleClickCommand.empty() && middleClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onMButtonDown((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_MBUTTONUP:
		{
			if(middleDoubleClickCommand.empty() && middleClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onMButtonUp((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_RBUTTONDBLCLK:
		{
			if(rightDoubleClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onRButtonDblClk((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_RBUTTONDOWN:
		{
			if(rightDoubleClickCommand.empty() && rightClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onRButtonDown((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_RBUTTONUP:
		{
			if(rightDoubleClickCommand.empty() && rightClickCommand.empty())
			{
				relayMouseMessageToBox(message, wParam, lParam);
				return true;
			}

			onRButtonUp((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_MOUSEWHEEL:
		{
			if ((short)(HIWORD(wParam)) < 0)
			{
				if(wheelDownCommand.empty())
				{
					relayMouseMessageToBox(message, wParam, lParam);
					return true;
				}

				onWheelDown((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
				return true;
			}
			else
			{
				if(wheelUpCommand.empty())
				{
					relayMouseMessageToBox(message, wParam, lParam);
					return true;
				}
				
				onWheelUp((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
				return true;
			}
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

			if(!wParam)
				hDC = BeginPaint(hWnd, &ps);
			else
				hDC = (HDC) wParam;

			onPaint(hDC);

			if(!wParam)
				EndPaint(hWnd, &ps);

			return true;
		}

		case WM_SIZE:
		{
			onSize((int) (short) LOWORD(lParam), (int) (short) HIWORD(lParam));
			return true;
		}

		case WM_TIMER:
		{
			onTimer((int) wParam);
			return true;
		}

		case WM_WINDOWPOSCHANGED:
		{
			onWindowPosChanged((WINDOWPOS *) lParam);
			return false;
		}
	}

	return false;
}

LRESULT Label::windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Label *label = NULL;

	if(message == WM_NCCREATE)
	{
		label = (Label *) ((CREATESTRUCT *) lParam)->lpCreateParams;
		label->hWnd = hWnd;
		SetWindowLong(hWnd, 0, (LONG) label);
	}
	else
		label = (Label *) GetWindowLong(hWnd, 0);

	if(label)
	{
		LRESULT lResult = 0;

		if(label->onWindowMessage(message, wParam, lParam, lResult))
			return lResult;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
