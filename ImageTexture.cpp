#include "common.h"
#include "ImageTexture.h"

ImageTexture::ImageTexture()
{
	bitmap = 0;

	memoryDC = 0;
	memoryBitmap = 0;
}

ImageTexture::~ImageTexture()
{
	if(bitmap != 0)
	{
		SelectObject(memoryDC, memoryBitmap);
		DeleteDC(memoryDC);
		DeleteObject(bitmap);
	}
}

NameValuePair modeConstants[] =
{
	{ "stretch", MULTIBLT_STRETCH },
	{ "tile", MULTIBLT_TILE },
	{ "tileHorizontal", MULTIBLT_TILEHORIZONTAL },
	{ "tileVertical", MULTIBLT_TILEVERTICAL },
	{ 0, 0 }
};

void ImageTexture::configure(const string &prefix)
{
	string imageFile = GetRCString(prefix, "Image", "");
	bitmap = LoadLSImage(imageFile.c_str(), 0);
	GetLSBitmapSize(bitmap, &imageWidth, &imageHeight);

	memoryDC = CreateCompatibleDC(0);
	memoryBitmap = (HBITMAP) SelectObject(memoryDC, bitmap);

	transparent = GetRCBoolean(prefix, "Transparent");

	leftEdge = GetRCInt(prefix, "ImageLeftEdge", 0, 0);
	topEdge = GetRCInt(prefix, "ImageTopEdge", 0, 0);
	rightEdge = GetRCInt(prefix, "ImageRightEdge", 0, 0);
	bottomEdge = GetRCInt(prefix, "ImageBottomEdge", 0, 0);

	mode = GetRCNamedValue(prefix, "ImageMode", modeConstants, MULTIBLT_STRETCH);
}

void ImageTexture::apply(HDC hDC, int x, int y, int width, int height)
{
	if(bitmap == 0)
		return;

	if(transparent)
	{
		HDC bufferDC = CreateCompatibleDC(hDC);
		HBITMAP bufferBitmap = CreateCompatibleBitmap(hDC, width, height);
		bufferBitmap = (HBITMAP) SelectObject(bufferDC, bufferBitmap);

		SetStretchBltMode(bufferDC, STRETCH_DELETESCANS);

		MultiBlt(bufferDC, 0, 0, width, height,
			memoryDC, 0, 0, imageWidth, imageHeight,
			leftEdge, topEdge, rightEdge, bottomEdge,
			mode, SRCCOPY);
		
		TransparentBltLS(hDC, x, y, width, height,
			bufferDC, 0, 0,
			RGB(255, 0, 255));
		
		bufferBitmap = (HBITMAP) SelectObject(bufferDC, bufferBitmap);
		DeleteDC(bufferDC);
		DeleteObject(bufferBitmap);
	}
	else
	{
		SetStretchBltMode(hDC, STRETCH_DELETESCANS);

		MultiBlt(hDC, x, y, width, height,
			memoryDC, 0, 0, imageWidth, imageHeight,
			leftEdge, topEdge, rightEdge, bottomEdge,
			mode, SRCCOPY);
	}
}

boolean ImageTexture::isTransparent() const
{
	return transparent;
}