#include "common.h"
#include "DefaultTexture.h"

void DefaultTexture::configure(const string &prefix, const string &subKey)
{
	// ...
}

void DefaultTexture::apply(HDC hDC, int x, int y, int width, int height)
{
	RECT r;

	r.left = x;
	r.top = y;
	r.right = r.left + width;
	r.bottom = r.top + height;

	DrawEdge(hDC,
		&r,
		EDGE_RAISED,
		BF_RECT | BF_MIDDLE);
}

boolean DefaultTexture::isTransparent() const
{
	return false;
}
