#if !defined(__BLANKTEXTURE_H)
#define __BLANKTEXTURE_H

#include "Texture.h"

class BlankTexture : public Texture
{
public:

	void configure(const string &prefix) { }
	void apply(HDC hDC, int x, int y, int width, int height) { }

	boolean isTransparent() const
	{
		return true;
	}
};

#endif
