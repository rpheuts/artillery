#ifndef FONT_H_
#define FONT_H_
#include "artillery.h"

class Font{
	public:
		static void Init();
		static void Destroy();
		static int GetFontWidth(int fontSize);
		static int PrintScreen(const char* text, int x, int y, Color color, int fontSize);
		static int PrintImage(const char* text, int x, int y, Image* image, Color color, int fontSize);
		static int PrintImage(char ch, int x, int y, Image* image, Color color, int fontSize);
		static int PrintScreen(const char* text, int x, int y, Color color, int fontSize, int alpha);
		static int PrintImage(const char* text, int x, int y, Image* image, Color color, int fontSize, int alpha);
	
	private:
		static int Print(const char* text, int x, int y, Color color, int fontSize, Color* dest, int line_size, int alpha);
		static int PrintLetter(char c, int x, int y, int fontSize, Color color, Color* dest, int line_size, int alpha);
		static int GetLetterWidth(char c, int fontSize);
		static int GetLetterPosition(char c, int fontSize);
};
#endif /*FONT_H_*/
