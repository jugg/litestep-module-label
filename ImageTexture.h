#if !defined(__IMAGETEXTURE_H)
#define __IMAGETEXTURE_H

#include "Texture.h"

class ImageTexture : public Texture
{
public:

	ImageTexture();
	virtual ~ImageTexture();

	void configure(const string &prefix, const string &subKey);
	void apply(HDC hDC, int x, int y, int width, int height);
	boolean isTransparent() const;

private:

	HBITMAP bitmap;

	HDC memoryDC;
	HBITMAP memoryBitmap;

	int imageHeight;
	int imageWidth;

	boolean transparent;

	int leftEdge;
	int topEdge;
	int rightEdge;
	int bottomEdge;

	int mode;
};

#endif
