#if !defined(__SOLIDTEXTURE_H)
#define __SOLIDTEXTURE_H

#include "Texture.h"

class SolidTexture : public Texture
{
public:

	SolidTexture();
	void read(const string &prefix);

	void apply(HDC hDC, int x, int y, int width, int height);
	boolean isTransparent() const;

private:

	int backgroundColor;
	int darkBevelColor;
	int lightBevelColor;
};

#endif
