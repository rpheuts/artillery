#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <psptypes.h>

// Defines
#define PSP_LINE_SIZE 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272
#define MK8888(b,g,r,a) (((b&0xFF)<<16) + ((g&0xFF)<<8) + ((r&0xFF)) + ((a&0xFF)<<24))
#define IS_ALPHA(color) ((color)&0xFF000000?0:1)
#define IS_RED(color) ((color)&0x000000FF?1:0)
#define GET_ALPHA(c) ((unsigned short)((c & 0xFF000000) >> 24))
#define GET_RED(c) ((unsigned short)((c & 0x000000FF)))
#define GET_GREEN(c) ((unsigned short)((c & 0x0000FF00) >> 8))
#define GET_BLUE(c) ((unsigned short)((c & 0x00FF0000)>>16))

//Typedefs
typedef u32 Color;

typedef struct
{
	int textureWidth;  // the real width of data, 2^n with n>=0
	int textureHeight;  // the real height of data, 2^n with n>=0
	int imageWidth;  // the image width
	int imageHeight;
	Color* data;
} Image;

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

class Graphics{
	public:
		static void Init();
		static Image* CreateImage(int width, int height);
		static Image* ResizeImage(int width, int height, Image* source);
		static void ResizeImage(int width, int height, Image* source, Image* destination);
		static Image* LoadPNG(const char * filename);
		static Image* LoadPNG(int dummy);
		static Image* LoadJPG(const char * filename);
		static Image* GrabScreenBuffer();
		static void FlipScreen();
		static void ClearScreen(Color color);
		static void BlitImageToImage(int sx, int sy, int width, int height, Image* source, int dx, int dy, Image* destination);
		static void BlitImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy);
		static void BlitAlphaImageToImage(int sx, int sy, int width, int height, Image* source, int dx, int dy, Image* destination);
		static void BlitAlphaImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy);
		static void SavePNG(const char* filename, Color* data, int width, int height, int lineSize, int saveAlpha);
		static void SaveScreenBuffer(const char * filename);
		static void FreeImage(Image* image);
		static void ClearImage(Color color, Image* image);
		static void DisableGraphics();
		static Color* GetVramDrawBuffer();
		static Color* GetVramDisplayBuffer();
		static void WaitVblankStart();
		static unsigned long Graphics::AlphaBlend(unsigned long front, unsigned long back);
		
	private:
		static int GetNextPower2(int width);
};
#endif
