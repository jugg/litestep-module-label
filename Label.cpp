#include "common.h"
#include "bangCommands.h"
#include "LabelSettings.h"
#include "Font.h"
#include "Label.h"
#include "SystemInfo.h"
#include "Texture.h"

#define TIMER_MOUSETRACK 1
#define TIMER_UPDATE 2
#define TIMER_SCROLL 3

extern LabelList labelList;

Label::Label(const string &name)
{
	this->name = name;

	mousePressed = false;
	mouseInside = false;

	hWnd = 0;
	hInstance = 0;
	box = 0;

	visible = false;

	backgroundDC = 0;
	bufferDC = 0;
	backgroundBitmap = 0;
	bufferBitmap = 0;

	background = 0;
	font = 0;

}

Label::~Label()
{
	RemoveBangCommands(name, bangCommands);

	if(hWnd != 0)
	{
		KillTimer(hWnd, TIMER_SCROLL);
		DestroyWindow(hWnd);
	}

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

	if(background != defaultSettings.skin) delete background;
	if(font != defaultSettings.font) delete font;
}

void Label::load(HINSTANCE hInstance, HWND box)
{
	this->hInstance = hInstance;
	this->box = box;

	reconfigure();
	
	AddBangCommands(name, bangCommands);
}

void Label::reconfigure()
{
	LabelSettings settings(name.c_str());

	setAlwaysOnTop(settings.alwaysOnTop);
	reposition(settings.x, settings.y, settings.width, settings.height);

	bUseFahrenheit = settings.bUseFahrenheit;
	bangCommands = settings.bangCommands;

	setBackground(settings.skin);
	setFont(settings.font);
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
	enterCommand = settings.enterCommand;
	leaveCommand = settings.leaveCommand;

	scrollPadLength = settings.scrollPadLength;
	scrollInterval = settings.scrollInterval;
	scrollSpeed = settings.scrollSpeed;
	scroll = settings.scroll;

	trueTransparency = settings.trueTransparency;

	if(!settings.startHidden)
		show();

	if(background->isTransparent() && trueTransparency)
	{
		//set the window region that pink areas aren't part of it
		HDC tmpDC=CreateCompatibleDC(NULL);
		HBITMAP tmpBmp = CreateBitmap(width, height, 1, 32, NULL);
		HGDIOBJ tmpObj = SelectObject(tmpDC, tmpBmp);
		background->apply(tmpDC, 0, 0, width, height);
		SelectObject(tmpDC, tmpObj);
		SetWindowRgn(hWnd, BitmapToRegion(tmpBmp, RGB(255, 0, 255), 0, 0, 0), false);
		DeleteObject(tmpBmp);
		DeleteObject(tmpObj);
		DeleteDC(tmpDC);
	}
}

void Label::setAlwaysOnTop(boolean alwaysOnTop)
{
	this->alwaysOnTop = alwaysOnTop;

	if(hWnd != 0 && box == 0)
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
	if (this->alwaysOnTop) this->setAlwaysOnTop(FALSE);
	
	this->box = newparent;

	SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE) &~ WS_POPUP)|WS_CHILD);
	SetParent(hWnd, newparent);
//	SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE) &~ WS_CHILD)|WS_POPUP);
}

void Label::setBackground(Texture *background)
{
	if(this->background != defaultSettings.skin)
		delete this->background;

	this->background = background;
	repaint(true);
}

void Label::setFont(Font *font)
{
	if(this->font != defaultSettings.font)
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
	this->text = systemInfo->processLabelText(text, this, &dynamicText);

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
		SetWindowPos(hWnd, 0, 0, 0, width, height,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
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
		hWnd = CreateWindowEx(box ? 0 : WS_EX_TOOLWINDOW,
			"LabelLS",
			name.c_str(),
			box ? WS_CHILD : WS_POPUP,
			x, y,
			width, height,
			box ? box : GetLitestepWnd(),
			0,
			hInstance,
			this);

		SetWindowLong(hWnd, GWL_USERDATA, 0x49474541);
		setAlwaysOnTop(alwaysOnTop);
		if(dynamicText) SetTimer(hWnd, TIMER_UPDATE, updateInterval, 0);
	}
	visible = true;
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);

	if(scroll)
		SetTimer(hWnd, TIMER_SCROLL, scrollInterval, 0);
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
		if(scrollPosition < 0)
			scrollPosition=textWidth + scrollPadLength;

		if(scrollPosition > textWidth + scrollPadLength )
			scrollPosition=0;

		HRGN oldRGN=NULL;
		GetClipRgn(bufferDC, oldRGN);
		HRGN newRGN=CreateRectRgn(leftBorder, topBorder, width - rightBorder, height - bottomBorder);
		SelectClipRgn(bufferDC, newRGN);
		DeleteObject(newRGN);

		font->apply(bufferDC,
			leftBorder,
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

			scrollPosition-=scrollSpeed;
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

			return true;
		}

		case WM_CLOSE:
		{
			lResult = 0;
			return true;
		}

		case WM_DESTROY:
		{
			if(box)
			{
				//hide or destroy is the Question - i take hide - blkhawk
				this->hide();
				this->setBox(0);

				//Uncomment these to destroy the Label when the parent box gets killed
				/*hWnd = 0;
				labelList.remove(this);
				delete this;*/
			}

			return false;
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
