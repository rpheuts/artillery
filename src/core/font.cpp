#include "artillery.h"
#include <string.h>

Image* fontSmall;
int fontSmallWidth = 10;
int fontSmallHeight = 10;
int init = 0;
int renderToScreen = 0;
int fontLetterSizes[4] = {4, 18, 29, 39};
int fontLetterDimensions[4] = {10, 7, 6, 5};
int fontLetterIWidth[4] = {6, 4, 3, 2};
int fontLetter1Width[4] = {5, 3, 2, 1};

void Font::Init()
{
	fontSmall = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_font").c_str());
	init=1;
}

void Font::Destroy()
{
	Graphics::FreeImage(fontSmall);
	init=0;
}

int Font::GetFontWidth(int fontSize)
{
	return fontLetterDimensions[fontSize];
}

int Font::GetLetterPosition(char c, int fontSize)
{
	switch (c)
	{
		case 'A':
		case 'a':
			return 0 * fontLetterDimensions[fontSize] + 4;
		break;
		case 'B':
		case 'b':
			return 1 * fontLetterDimensions[fontSize] + 5;
		break;
		case 'C':
		case 'c':
			return 2 * fontLetterDimensions[fontSize] + 6;
		break;
		case 'D':
		case 'd':
			return 3 * fontLetterDimensions[fontSize] + 7;
		break;
		case 'E':
		case 'e':
			return 4 * fontLetterDimensions[fontSize] + 8;
		break;
		case 'F':
		case 'f':
			return 5 * fontLetterDimensions[fontSize] + 9;
		break;
		case 'G':
		case 'g':
			return 6 * fontLetterDimensions[fontSize] + 10;
		break;
		case 'H':
		case 'h':
			return 7 * fontLetterDimensions[fontSize] + 11;
		break;
		case 'I':
		case 'i':
			return 8 * fontLetterDimensions[fontSize] + 12;
		break;
		case 'J':
		case 'j':
			return 8 * fontLetterDimensions[fontSize] + 14;
		break;
		case 'K':
		case 'k':
			return 9 * fontLetterDimensions[fontSize] + 15;
		break;
		case 'L':
		case 'l':
			return 10 * fontLetterDimensions[fontSize] + 16;
		break;
		case 'M':
		case 'm':
			return 11 * fontLetterDimensions[fontSize] + 17;
		break;
		case 'N':
		case 'n':
			return 12 * fontLetterDimensions[fontSize] + 18;
		break;
		case 'O':
		case 'o':
			return 13 * fontLetterDimensions[fontSize] + 19;
		break;
		case 'P':
		case 'p':
			return 14 * fontLetterDimensions[fontSize] + 20;
		break;
		case 'Q':
		case 'q':
			return 15 * fontLetterDimensions[fontSize] + 21;
		break;
		case 'R':
		case 'r':
			return 16 * fontLetterDimensions[fontSize] + 22;
		break;
		case 'S':
		case 's':
			return 17 * fontLetterDimensions[fontSize] + 23;
		break;
		case 'T':
		case 't':
			return 18 * fontLetterDimensions[fontSize] + 24;
		break;
		case 'U':
		case 'u':
			return 19 * fontLetterDimensions[fontSize] + 25;
		break;
		case 'V':
		case 'v':
			return 20 * fontLetterDimensions[fontSize] + 26;
		break;
		case 'W':
		case 'w':
			return 21 * fontLetterDimensions[fontSize] + 27;
		break;
		case 'X':
		case 'x':
			return 22 * fontLetterDimensions[fontSize] + 28;
		break;
		case 'Y':
		case 'y':
			return 23 * fontLetterDimensions[fontSize] + 29;
		break;
		case 'Z':
		case 'z':
			return 24 * fontLetterDimensions[fontSize] + 30;
		break;
		case '1':
			return 25 * fontLetterDimensions[fontSize] + 31;
		break;
		case '2':
			return 25 * fontLetterDimensions[fontSize] + 35;
		break;
		case '3':
			return 26 * fontLetterDimensions[fontSize] + 36;
		break;
		case '4':
			return 27 * fontLetterDimensions[fontSize] + 37;
		break;
		case '5':
			return 28 * fontLetterDimensions[fontSize] + 38;
		break;
		case '6':
			return 29 * fontLetterDimensions[fontSize] + 39;
		break;
		case '7':
			return 30 * fontLetterDimensions[fontSize] + 40;
		break;
		case '8':
			return 31 * fontLetterDimensions[fontSize] + 41;
		break;
		case '9':
			return 32 * fontLetterDimensions[fontSize] + 42;
		break;
		case '0':
			return 33 * fontLetterDimensions[fontSize] + 43;
		break;
		case '.':
			return 34 * fontLetterDimensions[fontSize] + 44;
		break;
		case ',':
			return 34 * fontLetterDimensions[fontSize] + 46;
		break;
		case ':':
			return 34 * fontLetterDimensions[fontSize] + 49;
		break;
		case ';':
			return 34 * fontLetterDimensions[fontSize] + 51;
		break;
		case '\'':
			return 34 * fontLetterDimensions[fontSize] + 53;
		break;
		case '\"':
			return 34 * fontLetterDimensions[fontSize] + 55;
		case '/':
			return 34 * fontLetterDimensions[fontSize] + 60;
		case '\\':
			return 35 * fontLetterDimensions[fontSize] + 61;
		case '_':
			return 36 * fontLetterDimensions[fontSize] + 62;
		case '?':
			return 37 * fontLetterDimensions[fontSize] + 63;
		case '!':
			return 38 * fontLetterDimensions[fontSize] + 64;
		case '@':
			return 38 * fontLetterDimensions[fontSize] + 66;
		case '+':
			return 39 * fontLetterDimensions[fontSize] + 67;
		case '=':
			return 41 * fontLetterDimensions[fontSize] + 68;
		case '-':
			return 42 * fontLetterDimensions[fontSize] + 69;
		case '&':
			return 43 * fontLetterDimensions[fontSize] + 70;
		default:
			return 53 * fontLetterDimensions[fontSize] + 76;
		break;
	}
	return 0;
}

int Font::GetLetterWidth(char c, int fontSize)
{
	// Special cases
	if (c == 'i' || c == 'I')
		return 1;
	if (c == '1')
		return fontLetterDimensions[fontSize] - fontLetter1Width[fontSize];
	
	// If it is a letter or number
	if ((c > 47 && c < 58) || (c > 64 && c < 91) || (c > 96 && 123))
		return fontLetterDimensions[fontSize];
		
	// It's a puctuation mark
	switch (c)
	{
		case '.':
			return 2;
		break;
		case ',':
			return 3;
		break;
		case ':':
			return 2;
		break;
		case ';':
			return 3;
		break;
		case '\'':
			return 2;
		break;
		case '\"':
			return 5;
		case '/':
			return fontLetterDimensions[fontSize];
		case '\\':
			return fontLetterDimensions[fontSize];
		case '_':
			return fontLetterDimensions[fontSize];
		case '?':
			return fontLetterDimensions[fontSize];
		case '!':
			return 2;
		case '@':
			return fontLetterDimensions[fontSize];
		case '+':
			return fontLetterDimensions[fontSize];
		case '=':
			return fontLetterDimensions[fontSize];
		case '-':
			return fontLetterDimensions[fontSize];
		case '&':
			return fontLetterDimensions[fontSize];
		default:
			return fontLetterDimensions[fontSize];
		break;
	}
	
	return 0;
}

int Font::PrintLetter(char c, int x, int y, int fontSize, Color color, Color* dest, int line_size, int alpha)
{
	Color *vram;
	Color *vram_ptr;
	Color *font;
	Color *font_ptr;
	
	// Position ourself at the x,y position where the texts needs to be
	vram = dest + x + y * line_size;

	// Position the font pointer to the place where the letter starts we want
	font = fontSmall->data + GetLetterPosition(c, fontSize) + fontLetterSizes[fontSize] * fontSmall->textureWidth;
	int width = GetLetterWidth(c, fontSize);
	
	// render the letter
	int i,j;
	for (i=0; i<fontLetterDimensions[fontSize]; i++)
	{
		//this is the y position loop
		vram_ptr = vram;
		font_ptr = font;
		for (j=0; j<width; j++)
		{
			//this is the x position loop
			if (IS_RED(*font_ptr))
			{
				if (alpha)
					*vram_ptr = Graphics::AlphaBlend(color, *vram_ptr);
				else
					*vram_ptr = color;
			}
			vram_ptr++;
			font_ptr++;
		}
		vram += line_size;
		font += fontSmall->textureWidth;
	}
	return width;
}

int Font::Print(const char* text, int x, int y, Color color, int fontSize, Color* dest, int line_size, int alpha)
{
	if (!init)
		Init();
		
	int i, carret, width;
	int length = strlen(text);
	
	carret = x;
	for (i=0; i<length; i++)
	{
		width = GetLetterWidth(text[i], fontSize);
		if (text[i] != ' ' && x > 0 && y > 0)
			carret += PrintLetter(text[i], carret, y, fontSize, color, dest, line_size, alpha) + 1;
		else if (text[i] == ' ')
			carret += fontLetterDimensions[fontSize];
	}
	return carret;
}

int Font::PrintScreen(const char* text, int x, int y, Color color, int fontSize, int alpha)
{
	return Print(text, x, y, color, fontSize, Graphics::GetVramDrawBuffer(), PSP_LINE_SIZE, alpha);
}

int Font::PrintScreen(const char* text, int x, int y, Color color, int fontSize)
{
	return Print(text, x, y, color, fontSize, Graphics::GetVramDrawBuffer(), PSP_LINE_SIZE, 0);
}

int Font::PrintImage(const char* text, int x, int y, Image* image, Color color, int fontSize, int alpha)
{
	return Print(text, x, y, color, fontSize, image->data, image->textureWidth, alpha);
}

int Font::PrintImage(char ch, int x, int y, Image* image, Color color, int fontSize)
{
	return PrintLetter(ch, x, y, fontSize, color, image->data, image->textureWidth, 0);
}

int Font::PrintImage(const char* text, int x, int y, Image* image, Color color, int fontSize)
{
	return Print(text, x, y, color, fontSize, image->data, image->textureWidth, 0);
}

