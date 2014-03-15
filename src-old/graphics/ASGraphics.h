/*
# PSP Framework 				
# ------------------------------------------------------------------
# Author: R.P.Heuts
# Date: 23-05-2006
# Version: 0.1
# 
# Class: Graphics
# Desciption:
# Provides interface and graphical functionality for blitting images
# and content to the screen. Uses the GU kernel functions for better
# performance.
*/

#ifndef ASGRAPHICS_H
#define ASGRAPHICS_H

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

class ASGraphics{
	public:
		static ASGraphics* Instance();
		
		// Returns the current draw buffer
		inline Color* GetVramDrawBuffer()
		{
			Color* vram = (Color*) _vramBase;
			if (_screenBuffNum == 0) vram += _frameBufferSize / sizeof(Color);
			return vram;
		}

		// Returns the current display buffer
		inline Color* GetVramDisplayBuffer()
		{
			Color* vram = (Color*) _vramBase;
			if (_screenBuffNum == 1) vram += _frameBufferSize / sizeof(Color);
			return vram;
		}
		
		// Returns an emtpy image (DEPRICATED, please instantiate image class yourself)
		inline ASImage* CreateImage(int width, int height)
		{
			return new ASImage(width, height);
		}

		// Frees an image from mem (DEPRICATED, please free image class yourself)
		inline void FreeImage(ASImage* image)
		{
			delete (image);
		}
		
		// Load a JPG or PNG image (judged by the extension)
		ASImage* Load(string filename);
		
		// Load a PNG image
		ASImage* LoadPNG(string filename);
		
		// Load a JPG image
		ASImage* LoadJPG(string filename);
		
		// Can resize one image and blit it to another newly created image leaving the original untouched
		ASImage* ResizeImage(int width, int height, ASImage* source);
		
		// Can resize one image and blit it to another image leaving the original untouched
		void ResizeImage(int width, int height, ASImage* source, ASImage* destination);
		
		// Clears the image with the specified color
		void ClearImage(Color color, ASImage* image);
		
		// Returns the current display buffer as an Image
		ASImage* GetDisplayBuffer();
		
		// Sets the current draw buffer to the display buffer, and the current display buffer to the draw buffer
		void FlipScreen();
		
		// Wait for VSync (screen is fully drawn)
		void WaitVblankStart();
		
		// Blit an image to the screen at xd,dy with size width,height
		void BlitImageToScreen(int sx, int sy, int width, int height, ASImage* source, int dx, int dy);
		
		// Blit an image containing alpha data to the screen
		void BlitAlphaImageToScreen(int sx, int sy, int width, int height, ASImage* source, int dx, int dy);
		
		// Blit an image to another image at the specified postition with specified width and height
		void BlitImageToImage(int sx, int sy, int width, int height, ASImage* source, int dx, int dy, ASImage* destination);
		
		// Blit an image to another image at the specified postition with specified width and height with alphablending
		void BlitAlphaImageToImage(int sx, int sy, int width, int height, ASImage* source, int dx, int dy, ASImage* destination);
		
		// Clear the screen with a specific color
		void ClearScreen(Color color);
		
		// Save an Image to a PNG file
		void SavePNG(string filename, ASImage* image, int saveAlpha);
		
		// Save raw data to a PNG file
		void SavePNG(string filename, COLORREF data, int width, int height, int saveAlpha);
		
		// Take a screenshot of the current display and save it
		void Screenshot(string filename);
		
		// Can stream load a png from a open file with id 'uid'
		ASImage* StreamPNG(int uid);
		
		// Can alpha blend (using the CPU) two parts together
		inline unsigned long ASGraphics::AlphaBlend(unsigned long front, unsigned long back)
		{
			unsigned short alpha = GET_ALPHA(front);
			unsigned short beta = 255 - alpha;

			return MK8888(
				((GET_BLUE(front) * alpha) + (GET_BLUE(back) * beta)) / 255,
				((GET_GREEN(front) * alpha) + (GET_GREEN(back) * beta)) / 255,
				((GET_RED(front) * alpha) + (GET_RED(back) * beta)) / 255,
				(GET_ALPHA(back)));
		}
		
	private:
		ASGraphics(int screenWidth, int screenHeight, int lineSize);
		
	private:
		static ASGraphics* _instance;
		int _screenWidth;
		int _screenHeight;
		int _screenLineSize;
		int _screenBuffNum;
		int _frameBufferSize;
		Color* _vramBase;
		unsigned int __attribute__((aligned(16))) _gulist[262144];
};
#endif
