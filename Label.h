#if !defined(__LABEL_H)
#define __LABEL_H

class Font;
class Texture;

class Label
{
public:

	Label(const string &name);
	virtual ~Label();

	void load(HINSTANCE hInstance, HWND box = 0);
	void reconfigure();

	Texture *getBackground() const { return background; }
	Font *getFont() const { return font; }

	boolean isAlwaysOnTop() const { return alwaysOnTop; }
	boolean isVisible() const { return visible; }
	boolean isScrolling() const { return scroll; }
	boolean bUseFahrenheit;

	int bangCommands;		// probably shouldn't be public but i'm lazy...
	
	int getHeight() const { return height; }
	int getWidth() const { return width; }
	int getX() const { return x; }
	int getY() const { return y; }

	int getUpdateInterval() { return updateInterval; }
	int getJustify() const { return justify; }

	int getLeftBorder() const { return leftBorder; }
	int getTopBorder() const { return topBorder; }
	int getRightBorder() const { return rightBorder; }
	int getBottomBorder() const { return bottomBorder; }

	const HWND &getBox() const { return box; }
	const string &getName() const { return name; }
	const string &getText() const { return text; }

	void setAlwaysOnTop(boolean alwaysOnTop);
	void setBox(HWND newparent);
	void setBackground(Texture *background);
	void setFont(Font *font);
	void setJustify(int justify);
	void setText(const string &text);
	void setUpdateInterval(int updateInterval);
	void setLeftBorder(int leftBorder);
	void setTopBorder(int topBorder);
	void setRightBorder(int rightBorder);
	void setBottomBorder(int bottomBorder);
	void setScrolling(boolean scrolling);
	void setScrollLimit(int limit);

	void repaint(boolean invalidateCache = false);

	void move(int x, int y);
	void reposition(int x, int y, int width, int height);
	void resize(int width, int height);

	void hide();
	void show();

	void previous();
	void next();
	void update();

private:

	HWND hWnd;
	HINSTANCE hInstance;
	HWND box;

	HDC backgroundDC;
	HDC bufferDC;
	HBITMAP backgroundBitmap;
	HBITMAP bufferBitmap;

	Texture *background;
	Font *font;

	boolean alwaysOnTop;
	boolean visible;
	boolean bUsingDefSkin;
	boolean bInDestructor;

	int height;
	int width;
	int x;
	int y;

	int shadowY; // V

	int scrollPadLength;
	int scrollInterval;
	int scrollSpeed;
	boolean scroll;
	int scrollPosition;
	UINT scrollLimit;

//	boolean trueTransparency;

	int updateInterval;
	int justify;

	int leftBorder;
	int topBorder;
	int rightBorder;
	int bottomBorder;

	string name;

	boolean dynamicText;
	vector<string> originalText;
	int current;
	string text;

	boolean mousePressed;
	boolean mouseInside;

	string leftClickCommand;
	string leftDoubleClickCommand;
	string middleClickCommand;
	string middleDoubleClickCommand;
	string rightClickCommand;
	string rightDoubleClickCommand;
	string wheelDownCommand;
	string wheelUpCommand;
	string enterCommand;
	string leaveCommand;
	string dropCommand;

protected:

	void relayMouseMessageToBox(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void onLButtonDblClk(int x, int y);
	virtual void onLButtonDown(int x, int y);
	virtual void onLButtonUp(int x, int y);
	virtual void onMButtonDblClk(int x, int y);
	virtual void onMButtonDown(int x, int y);
	virtual void onMButtonUp(int x, int y);
	virtual void onRButtonDblClk(int x, int y);
	virtual void onRButtonDown(int x, int y);
	virtual void onRButtonUp(int x, int y);
	virtual void onWheelDown(int x, int y);
	virtual void onWheelUp(int x, int y);
	virtual void onMouseEnter();
	virtual void onMouseLeave();
	virtual void onMouseMove(int x, int y);
	virtual void onDrop(const string &file);
	virtual void onPaint(HDC hDC);
	virtual void onSize(int width, int height);
	virtual void onTimer(int timerID);
	virtual void onWindowPosChanged(WINDOWPOS *windowPos);

	virtual boolean onWindowMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT &lResult);

public:

	static LRESULT WINAPI windowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
