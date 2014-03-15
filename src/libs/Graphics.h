#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Includes.h"

class Graphics
{
	public:
		static Graphics* Instance();
		Image* Load(File* filename);
		Image* Load(string filename);
		int GetFontWidth(int fontSize);
		int GetFontHeight(int fontSize);
		int PrintScreen(string text, int x, int y, Color color, int fontSize, int fontType);
		int PrintImage(string text, int x, int y, Image* image, Color color, int fontSize, int fontType);
		int PrintScreen(string text, int x, int y, Color color, int fontSize);
		int PrintImage(string text, int x, int y, Image* image, Color color, int fontSize);
		int PrintImage(char ch, int x, int y, Image* image, Color color, int fontSize, int fontType);
		int GetCharacterWidth(char c, int fontSize, int fontType);
		int GetStringWidth(string s, int fontSize, int fontType);
		Image* SaveFramebuffer();
		
		// Can resize one image and blit it to another newly created image leaving the original untouched
		Image* ResizeImage(int width, int height, Image* source);
		
		// Can resize one image and blit it to another image leaving the original untouched
		void ResizeImage(int width, int height, Image* source, Image* destination);
		
	private:
		Graphics();
		~Graphics();
		Image* LoadImage(File* file);
		Image* LoadPNG(File* filename);
		Image* LoadJPG(File* filename);
		int GetCharacterPositionX(char c, int fontSize);
		int GetCharacterPositionY(char c, int fontSize, int fontType);
		int GetPunctuationPositionX(char c, int fontSize);
		int PrintLetter(char c, int x, int y, int fontSize, int fontType, Color color, Color* dest, int line_size, int alpha);
		int Print(const char* text, int x, int y, Color color, int fontSize, int fontType, Color* dest, int line_size, int alpha);
		
		// Can alpha blend (using the CPU) two images together
		inline unsigned long Graphics::AlphaBlend(unsigned long front, unsigned long back)
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
		static Graphics* _instance;
		int _gridWidth[4];
		int _gridHeight[4];
		int _fontWidth[4];
		int _fontTypePostionCapital[3];
		int _fontTypePostionSmall[3];
		int _fontTypePostionPunctuation[3];
		Image* _font0;
		Image* _font1;
		Image* _font2;
};

#endif
