#if !defined(__DEFAULTTEXTURE_H)
#define __DEFAULTTEXTURE_H

#include "Texture.h"

class DefaultTexture : public Texture
{
public:

	void configure(const string &prefix, const string &subKey);
	void apply(HDC hDC, int x, int y, int width, int height);
	boolean isTransparent() const;
};

#endif
