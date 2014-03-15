#ifndef IMAGE_H
#define IMAGE_H

#include "Includes.h"

#define MK8888(b,g,r,a) (((b&0xFF)<<16) + ((g&0xFF)<<8) + ((r&0xFF)) + ((a&0xFF)<<24))
#define IS_ALPHA(color) ((color)&0xFF000000?0:1)
#define IS_RED(color) ((color)&0x000000FF?1:0)
#define GET_ALPHA(c) ((unsigned short)((c & 0xFF000000) >> 24))
#define GET_RED(c) ((unsigned short)((c & 0x000000FF)))
#define GET_GREEN(c) ((unsigned short)((c & 0x0000FF00) >> 8))
#define GET_BLUE(c) ((unsigned short)((c & 0x00FF0000)>>16))

enum ImageEffect {ImageBlur, ImageFade, ImageAlpha};

class Image
{
	//Typedefs
	typedef struct
	{
		unsigned short u, v;
		short x, y, z;
	} Vertex;

	typedef struct
	{
		int x; 
		int y;
	} Position;
	
	public:
		Image(int width, int height);
		~Image();
		Color* GetSurface();
		void Clear(Color color);
		void BlitToScreen(int targetX, int targetY);
		void BlitToScreen(int width, int height, int targetX, int targetY);
		void BlitToScreen(int sourceX, int sourceY, int width, int height, int targetX, int targetY);
		void AlphaBlitToScreen(int targetX, int targetY);
		void AlphaBlitToScreen(int width, int height, int targetX, int targetY);
		void AlphaBlitToScreen(int sourceX, int sourceY, int width, int height, int targetX, int targetY);
		void BlitToImage(int targetX, int targetY, Image* target);
		void BlitToImage(int width, int height, int targetX, int targetY, Image* target);
		void BlitToImage(int sourceX, int sourceY, int width, int height, int targetX, int targetY, Image* target);
		void AlphaBlitToImage(int targetX, int targetY, Image* target);
		void AlphaBlitToImage(int width, int height, int targetX, int targetY, Image* target);
		void AlphaBlitToImage(int width, int height, int sourceX, int sourceY, int targetX, int targetY, Image* target);
		int Print(string text, int x, int y, Color color, int fontSize, int fontType);
		int Print(string text, int x, int y, Color color, int fontSize);
		int Print(char ch, int x, int y, Color color, int fontSize, int fontType);
		void ApplyEffect(ImageEffect effect);
		void ApplyEffect(ImageEffect effect, int amount);
		void ApplyFade(int fadeAmount, bool set);
		void ApplyBlur();
		
	protected:
		int GetNextPower2(int number);
		
		// Can alpha blend (using the CPU) two images together
		inline unsigned long Image::AlphaBlend(unsigned long front, unsigned long back)
		{
			unsigned short alpha = GET_ALPHA(front);
			unsigned short beta = 255 - alpha;

			return MK8888(
					((GET_BLUE(front) * alpha) + (GET_BLUE(back) * beta)) / 255,
			((GET_GREEN(front) * alpha) + (GET_GREEN(back) * beta)) / 255,
			((GET_RED(front) * alpha) + (GET_RED(back) * beta)) / 255,
			(GET_ALPHA(back)));
		}
		
	public:
		int Width;
		int Height;
		int TextureWidth;
		int TextureHeight;
		
	protected:
		Color* _surface;
};

#endif
