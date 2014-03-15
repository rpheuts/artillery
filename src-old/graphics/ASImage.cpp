#include <malloc.h>
#include "ASIncludes.h"

ASImage::ASImage(int imageWidth, int imageHeight)
{
	Width = imageWidth;
	Height = imageHeight;
	
	// Set the texture width, needed for blitting
	TextureWidth = GetNextPower2(Width);
	TextureHeight = GetNextPower2(Height);
        
        // Init the surface
	Surface = (Color*) memalign(16, TextureWidth * TextureHeight * sizeof(Color));
	if (!Surface)
		return;
	
	memset(Surface, 0, TextureWidth * TextureHeight * sizeof(Color));
}

ASImage::~ASImage()
{
	free (Surface);
}

void ASImage::Clear(Color color)
{
	int size = TextureWidth * TextureHeight;
	for (int i = 0; i < size; i++, Surface++) *Surface = color;
}

// Used for calculating the actual texture width (power of 2) for blitting
int ASImage::GetNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}
