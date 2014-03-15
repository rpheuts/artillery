#ifndef FONT_H_
#define FONT_H_
#include "ASIncludes.h"

class ASFont{
	public:
		ASFont();
		~ASFont();
		int GetFontWidth(int fontSize);
		int GetFontHeight(int fontSize);
		int PrintScreen(const char* text, int x, int y, Color color, int fontSize, int fontType);
		int PrintImage(const char* text, int x, int y, ASImage* image, Color color, int fontSize, int fontType);
		int PrintScreen(string text, int x, int y, Color color, int fontSize, int fontType);
		int PrintImage(string text, int x, int y, ASImage* image, Color color, int fontSize, int fontType);
		int PrintScreen(string text, int x, int y, Color color, int fontSize);
		int PrintImage(string text, int x, int y, ASImage* image, Color color, int fontSize);
		int PrintImage(char ch, int x, int y, ASImage* image, Color color, int fontSize, int fontType);
		int PrintScreen(const char* text, int x, int y, Color color, int fontSize, int fontType, int alpha);
		int PrintImage(const char* text, int x, int y, ASImage* image, Color color, int fontSize, int fontType, int alpha);
		int GetCharacterWidth(char c, int fontSize, int fontType);

	private:
		int GetCharacterPositionX(char c, int fontSize);
		int GetCharacterPositionY(char c, int fontSize, int fontType);
		int GetPunctuationPositionX(char c, int fontSize);
		int PrintLetter(char c, int x, int y, int fontSize, int fontType, Color color, COLORREF dest, int line_size, int alpha);
		int Print(const char* text, int x, int y, Color color, int fontSize, int fontType, COLORREF dest, int line_size, int alpha);
	
	private:
		int _gridWidth[4];
		int _gridHeight[4];
		int _fontWidth[4];
		int _fontTypePostionCapital[3];
		int _fontTypePostionSmall[3];
		int _fontTypePostionPunctuation[3];
		ASImage* _font0;
		ASImage* _font1;
		ASImage* _font2;
};
#endif /*FONT_H_*/
