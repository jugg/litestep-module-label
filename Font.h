#if !defined(__FONT_H)
#define __FONT_H

class Font
{
public:

	Font();
	virtual ~Font();

	void configure(const string &prefix, Font *baseFont);
	void apply(HDC hDC, int x, int y, int width, int height, const string &text, unsigned int flags);
	void measure(HDC hDC, const string &text, unsigned int flags, int *width, int *height);
	
	void setColor(int aColor);
	
private:

	HFONT hFont;

	string name;
	int height;
	int color;

	boolean bold;
	boolean italic;

	boolean shadow;
	int shadowColor;
	int shadowX;
	int shadowY;

	void createHandle();
};

#endif
