#include "common.h"
#include "ImageTexture.h"

ImageTexture::ImageTexture()
{
	bitmap = 0;

	memoryDC = 0;
	memoryBitmap = 0;
}

ImageTexture::~ImageTexture()	// it seems this destructor is never called
{
	if(bitmap)
	{
//		bitmap = (HBITMAP) SelectObject(memoryDC, memoryBitmap);
		SelectObject(memoryDC, memoryBitmap);
		DeleteObject(bitmap);
	}
	if(memoryDC)
		DeleteDC(memoryDC);
}

NameValuePair modeConstants[] =
{
	{ "stretch", MULTIBLT_STRETCH },
	{ "tile", MULTIBLT_TILE },
	{ "tile-horizontal", MULTIBLT_TILEHORIZONTAL },
	{ "tile-vertical", MULTIBLT_TILEVERTICAL },
	{ "tilehorizontal", MULTIBLT_TILEHORIZONTAL },
	{ "tilevertical", MULTIBLT_TILEVERTICAL },
	{ 0, 0 }
};

void ImageTexture::configure(const string &prefix, const string &subKey)
{
	string imageFile = GetRCString(prefix, "Image", "");
	bitmap = LoadLSImage(imageFile.c_str(), 0);
	GetLSBitmapSize(bitmap, &imageWidth, &imageHeight);

	memoryDC = CreateCompatibleDC(0);
	memoryBitmap = (HBITMAP) SelectObject(memoryDC, bitmap);

	transparent = GetRCBoolean(prefix, "Transparent");
	trueTransparency = GetRCBoolean(prefix, "TrueTransparency");

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

	if(transparent && !trueTransparency)
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
