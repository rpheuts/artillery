#include "ASIncludes.h"
#include <string.h>

ASFont::ASFont()
{
	
	_font0 = ASGraphics::Instance()->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_font0").c_str());
	_font1 = ASGraphics::Instance()->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_font1").c_str());
	_font2 = ASGraphics::Instance()->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_font2").c_str());
	_gridWidth[0] = 9;
	_gridHeight[0] = 9;
	_fontWidth[0] = 4;
	_gridWidth[1] = 12;
	_gridHeight[1] = 10;
	_fontWidth[1] = 5;
	_gridWidth[2] = 12;
	_gridHeight[2] = 12;
	_fontWidth[2] = 6;
	_fontTypePostionCapital[0] = 0;
	_fontTypePostionCapital[1] = 3;
	_fontTypePostionCapital[2] = 6;
	_fontTypePostionSmall[0] = 1;
	_fontTypePostionSmall[1] = 4;
	_fontTypePostionSmall[2] = 7;
	_fontTypePostionPunctuation[0] = 2;
	_fontTypePostionPunctuation[1] = 5;
	_fontTypePostionPunctuation[2] = 8;
}

ASFont::~ASFont()
{
	ASGraphics::Instance()->FreeImage(_font0);
	ASGraphics::Instance()->FreeImage(_font1);
	ASGraphics::Instance()->FreeImage(_font2);
}

int ASFont::GetFontWidth(int fontSize)
{
	return _fontWidth[fontSize];
}

int ASFont::GetFontHeight(int fontSize)
{
	return _gridHeight[fontSize];
}

int ASFont::GetCharacterPositionX(char c, int fontSize)
{
	// Cast char to int
	int i = (int)c;

	if (c > 64 && c < 91)
		return (i-65) * _gridWidth[fontSize];
	if (c > 96 && c < 123)
		return (i-97) * _gridWidth[fontSize];
	if (c > 47 && c < 58)
		return (i-22) * _gridWidth[fontSize];

	switch (c)
	{
		case '.':
			return 0 * _gridWidth[fontSize];
		break;
		case ',':
			return 1 * _gridWidth[fontSize];
		break;
		case ':':
			return 2 * _gridWidth[fontSize];
		break;
		case ';':
			return 3 * _gridWidth[fontSize];
		break;
		case '\'':
			return 4 * _gridWidth[fontSize];
		break;
		case '\"':
			return 5 * _gridWidth[fontSize];
		case '/':
			return 6 * _gridWidth[fontSize];
		case '\\':
			return 7 * _gridWidth[fontSize];
		case '_':
			return 8 * _gridWidth[fontSize];
		case '?':
			return 9 * _gridWidth[fontSize];
		case '!':
			return 10 * _gridWidth[fontSize];
		case '@':
			return 11 * _gridWidth[fontSize];
		case '+':
			return 12 * _gridWidth[fontSize];
		case '-':
			return 13 * _gridWidth[fontSize];
		case '=':
			return 14 * _gridWidth[fontSize];
		case '<':
			return 15 * _gridWidth[fontSize];
		case '>':
			return 16 * _gridWidth[fontSize];
		case '[':
			return 17 * _gridWidth[fontSize];
		case ']':
			return 18 * _gridWidth[fontSize];
		case '(':
			return 19 * _gridWidth[fontSize];
		case ')':
			return 20 * _gridWidth[fontSize];
		case '%':
			return 21 * _gridWidth[fontSize];
		default:
			return 0;
	}
	return 0;
}

int ASFont::GetCharacterPositionY(char c, int fontSize, int fontType)
{
	if (c > 64 && c < 91)
		return _gridHeight[fontSize] * _fontTypePostionCapital[fontType];
	if ((c > 96 && c < 123) || (c > 47 && c < 58))
		return _gridHeight[fontSize] * _fontTypePostionSmall[fontType];
	if ((c > 32 && c < 48) || (c > 57 && c < 97) || (c > 122 && c < 127))
		return _gridHeight[fontSize] * _fontTypePostionPunctuation[fontType];
	return 0;
}

int ASFont::GetCharacterWidth(char c, int fontSize, int fontType)
{
	int typeWidth = 0;
	if (fontType != 0)
		typeWidth = 1;

	switch (c)
	{
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'G':
		case 'H':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'X':
		case 'Y':
		case 'v':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
		case '=':
			return typeWidth + _fontWidth[fontSize] + 1;
		break;
		case 'E':
		case 'F':
		case 'L':
		case 'Z':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'g':
		case 'h':
		case 'k':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 's':
		case 'u':
		case '/':
		case '\\':
		case '_':
		case '-':
		case '?':
		case '<':
		case '>':
			return typeWidth + _fontWidth[fontSize];
		case 'M':
		case 'W':
		case 'm':
		case 'w':
		case '@':
			if (fontSize == 2) // font fix
				return typeWidth + _fontWidth[fontSize] + 4;
			else
				return typeWidth + _fontWidth[fontSize] + 3;
		case 'J':
		case 'f':
		case 't':
		case 'x':
		case 'z':
		case '\"':
			return typeWidth + _fontWidth[fontSize] - 1;
		case 'I':
		case 'i':
		case 'l':
		case '.':
		case ':':
		case '!':
		case '\'':
			return typeWidth + _fontWidth[fontSize] - 3;
		case 'K':
		case 'N':
		case '%':
			return typeWidth + _fontWidth[fontSize] + 2;
		case 'j':
		case 'r':
		case ',':
		case ';':
		case '[':
		case ']':
		case '(':
		case ')':
			return typeWidth + _fontWidth[fontSize] - 2;
		case 'y':
			if (fontSize == 0) // font fix
				return typeWidth + _fontWidth[fontSize] - 1;
			else
				return typeWidth + _fontWidth[fontSize];
		default:
			return typeWidth + _fontWidth[fontSize];
	}
	
	return 0;
}

int ASFont::PrintLetter(char c, int x, int y, int fontSize, int fontType, Color color, Color* dest, int line_size, int alpha)
{
	Color *vram;
	Color *vram_ptr;
	Color *font = 0;
	Color *font_ptr;
	
	// Position ourself at the x,y position where the texts needs to be
	vram = dest + x + y * line_size;

	// Position the font pointer to the place where the letter starts we want
	if (fontSize == 0)
		font = _font0->Surface + GetCharacterPositionX(c, fontSize)+1 + GetCharacterPositionY(c, fontSize, fontType) * _font0->TextureWidth;
	else if (fontSize == 1)
		font = _font1->Surface + GetCharacterPositionX(c, fontSize)+1 + GetCharacterPositionY(c, fontSize, fontType) * _font1->TextureWidth;
	else if (fontSize == 2)
		font = _font2->Surface + GetCharacterPositionX(c, fontSize)+1 + GetCharacterPositionY(c, fontSize, fontType) * _font2->TextureWidth;
	int width = GetCharacterWidth(c, fontSize, fontType);
	
	// render the letter
	int i,j;
	for (i=_gridHeight[fontSize]-1; i>=0; i--)
	{
		//this is the y position loop
		vram_ptr = vram;
		font_ptr = font;
		for (j=width-1; j>= 0; j--)
		{
			//this is the x position loop
			if (IS_RED(*font_ptr))
			{
				if (alpha)
					*vram_ptr = ASGraphics::Instance()->AlphaBlend(color, *vram_ptr);
				else
					*vram_ptr = color;
			}
			++vram_ptr;
			++font_ptr;
		}
		vram += line_size;
		font += _font0->TextureWidth;
	}
	return width;
}

int ASFont::Print(const char* text, int x, int y, Color color, int fontSize, int fontType, Color* dest, int line_size, int alpha)
{
	int i, carret;
	int length = strlen(text);
	
	carret = x;
	for (i=0; i<length; i++)
	{
		if (text[i] != ' ' && x > 0 && y > 0)
			carret += PrintLetter(text[i], carret, y, fontSize, fontType, color, dest, line_size, alpha) + 1;
		else if (text[i] == ' ')
			carret += _fontWidth[fontSize];
	}
	return carret;
}

int ASFont::PrintScreen(const char* text, int x, int y, Color color, int fontSize, int fontType, int alpha)
{
	return Print(text, x, y, color, fontSize, fontType, ASGraphics::Instance()->GetVramDrawBuffer(), PSP_LINE_SIZE, alpha);
}

int ASFont::PrintScreen(const char* text, int x, int y, Color color, int fontSize, int fontType)
{
	return Print(text, x, y, color, fontSize, fontType, ASGraphics::Instance()->GetVramDrawBuffer(), PSP_LINE_SIZE, 0);
}

int ASFont::PrintImage(const char* text, int x, int y, ASImage* image, Color color, int fontSize, int fontType, int alpha)
{
	return Print(text, x, y, color, fontSize, fontType, image->Surface, image->TextureWidth, alpha);
}

int ASFont::PrintScreen(string text, int x, int y, Color color, int fontSize, int fontType)
{
	return Print(text.c_str(), x, y, color, fontSize, fontType, ASGraphics::Instance()->GetVramDrawBuffer(), PSP_LINE_SIZE, 0);
}

int ASFont::PrintImage(string text, int x, int y, ASImage* image, Color color, int fontSize, int fontType)
{
	return Print(text.c_str(), x, y, color, fontSize, fontType, image->Surface, image->TextureWidth, 0);
}

int ASFont::PrintScreen(string text, int x, int y, Color color, int fontSize)
{
	return Print(text.c_str(), x, y, color, fontSize, 0, ASGraphics::Instance()->GetVramDrawBuffer(), PSP_LINE_SIZE, 0);
}

int ASFont::PrintImage(string text, int x, int y, ASImage* image, Color color, int fontSize)
{
	return Print(text.c_str(), x, y, color, fontSize, 0, image->Surface, image->TextureWidth, 0);
}

int ASFont::PrintImage(char ch, int x, int y, ASImage* image, Color color, int fontSize, int fontType)
{
	return PrintLetter(ch, x, y, fontSize, fontType, color, image->Surface, image->TextureWidth, 0);
}

int ASFont::PrintImage(const char* text, int x, int y, ASImage* image, Color color, int fontSize, int fontType)
{
	return Print(text, x, y, color, fontSize, fontType, image->Surface, image->TextureWidth, 0);
}

