#if !defined(__TEXTURE_H)
#define __TEXTURE_H

class Texture
{
public:

	virtual void configure(const string &prefix) = 0;
	virtual void apply(HDC hDC, int x, int y, int width, int height) = 0;
	virtual boolean isTransparent() const = 0;
};

#endif
