#include "Includes.h"
#include <png.h>
#include <pspiofilemgr.h>

extern "C" {
#include <jpeglib.h>
#include <jerror.h>
}

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define SLICE_SIZE 32 // change this to experiment with different page-cache sizes

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

Graphics* Graphics::_instance = 0;

Graphics* Graphics::Instance()
{
	if (_instance == 0)
		_instance = new Graphics();
	return _instance;
}

Graphics::Graphics()
{
	File* font = new File(Core::MainConfigTheme->GetPathValue("c_font0"));
	_font0 = Load(font);
	delete (font);
	font = new File(Core::MainConfigTheme->GetPathValue("c_font1"));
	_font1 = Load(font);
	delete (font);
	font = new File(Core::MainConfigTheme->GetPathValue("c_font2"));
	_font2 = Load(font);
	delete (font);
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

Graphics::~Graphics()
{
	
}

Image* Graphics::Load(File* file)
{
	return LoadImage(file);
}

Image* Graphics::Load(string filename)
{
	File* file = new File(filename);
	Image* image = LoadImage(file);
	delete (file);
	return image;
}

Image* Graphics::LoadImage(File* file)
{
	if (file->Extension == "PNG")
		return LoadPNG(file);
	else if (file->Extension == "JPG")
		return LoadJPG(file);
	return NULL;
}

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{

}
		
Image* Graphics::LoadPNG(File* filename)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height, x, y;
	int bit_depth, color_type, interlace_type;
	u32* line;
	FILE *fp;

	if ((fp = fopen(filename->FullName.c_str(), "rb")) == NULL) return NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return NULL;
	}
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	
	Image* image = new Image(width, height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	if (!image->GetSurface()) {
		delete(image);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	line = (u32*) malloc(width * 4);
	if (!line) {
		delete(image);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	for (y = 0; y < height; y++) {
		png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
		for (x = 0; x < width; x++) {
			u32 color = line[x];
			image->GetSurface()[x + y * image->TextureWidth] =  color;
		}
	}
	free(line);
	// png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);
	
	return image;
}

Image* Graphics::LoadJPG(File* filename)
{
	struct jpeg_decompress_struct dinfo;
	struct jpeg_error_mgr jerr;
	dinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&dinfo);
	FILE* inFile = fopen(filename->FullName.c_str(), "rb");
	if (!inFile) {
		jpeg_destroy_decompress(&dinfo);
		return NULL;
	}
	jpeg_stdio_src(&dinfo, inFile);
	jpeg_read_header(&dinfo, TRUE);
	int width = dinfo.image_width;
	int height = dinfo.image_height;
	jpeg_start_decompress(&dinfo);
	Image* image = new Image(width, height);
	if (!image) {
		jpeg_destroy_decompress(&dinfo);
		fclose(inFile);
		return NULL;
	}
	u8* line = (u8*) malloc(width * 3);
	if (!line) {
		delete(image);
		jpeg_destroy_decompress(&dinfo);
		fclose(inFile);
		return NULL;
	}
	int x;
	if (dinfo.jpeg_color_space == JCS_GRAYSCALE) {
		while (dinfo.output_scanline < dinfo.output_height) {
			int y = dinfo.output_scanline;
			jpeg_read_scanlines(&dinfo, &line, 1);
			for (x = 0; x < width; x++) {
				Color c = line[x];
				image->GetSurface()[x + image->TextureWidth * y] = c | (c << 8) | (c << 16) | 0xff000000;;
			}
		}
	} else {
		while (dinfo.output_scanline < dinfo.output_height) {
			int y = dinfo.output_scanline;
			jpeg_read_scanlines(&dinfo, &line, 1);
			u8* linePointer = line;
			for (x = 0; x < width; x++) {
				Color c = *(linePointer++);
				c |= (*(linePointer++)) << 8;
				c |= (*(linePointer++)) << 16;
				image->GetSurface()[x + image->TextureWidth * y] = c | 0xff000000;
			}
		}
	}
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
	free(line);
	fclose(inFile);
	return image;
}

Image* Graphics::ResizeImage(int width, int height, Image* source)
{
	if (source == NULL)
		return NULL;
		
	Image* destination = new Image(width, height);
	ResizeImage(width, height, source, destination);
	return destination;
}
		
void Graphics::ResizeImage(int width, int height, Image* source, Image* destination)
{
	if (source == NULL)
		return;
	
	Color* destinationData = destination->GetSurface();
	int destinationSkipX = destination->TextureWidth - width;
	Color* sourceData = source->GetSurface();
	float scaleFactorX = ((float)source->Width) / ((float)width);
	float scaleFactorY = ((float)source->Height) / ((float)height);
	
	int x, y;
	for (y = 0; y < height; y++, destinationData += destinationSkipX) {
		for (x = 0; x < width; x++, destinationData++) {
			*destinationData = sourceData[(int)((int)(scaleFactorY * y) * source->TextureWidth) + (int)(x * scaleFactorX)];
		}
	}
}

Image* Graphics::SaveFramebuffer()
{
	ScreenDriver* screenDriver = ScreenDriver::Instance();
	Image* target = new Image(SCREEN_WIDTH, SCREEN_HEIGHT);
	Color* destinationData = &target->GetSurface()[target->TextureWidth];
	int destinationSkipX = target->TextureWidth - SCREEN_WIDTH;
	Color* sourceData = &screenDriver->GetVramDisplayPointer()[screenDriver->GetScreenLineSize()];
	int sourceSkipX = screenDriver->GetScreenLineSize() - SCREEN_WIDTH;
	int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (x = 0; x < SCREEN_WIDTH; x++, destinationData++, sourceData++) {
			*destinationData = *sourceData;
		}
	}
	return target;
}

int Graphics::GetFontWidth(int fontSize)
{
	return _fontWidth[fontSize];
}

int Graphics::GetFontHeight(int fontSize)
{
	return _gridHeight[fontSize];
}

int Graphics::GetCharacterPositionX(char c, int fontSize)
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

int Graphics::GetCharacterPositionY(char c, int fontSize, int fontType)
{
	if (c > 64 && c < 91)
		return _gridHeight[fontSize] * _fontTypePostionCapital[fontType];
	if ((c > 96 && c < 123) || (c > 47 && c < 58))
		return _gridHeight[fontSize] * _fontTypePostionSmall[fontType];
	if ((c > 32 && c < 48) || (c > 57 && c < 97) || (c > 122 && c < 127))
		return _gridHeight[fontSize] * _fontTypePostionPunctuation[fontType];
	return 0;
}

int Graphics::GetCharacterWidth(char c, int fontSize, int fontType)
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

int Graphics::GetStringWidth(string s, int fontSize, int fontType)
{
	int width = 0;
	for (unsigned int i = 0; i < s.size(); i++)
		width += GetCharacterWidth(s[i], fontSize, fontType);
	return width;
}

int Graphics::PrintLetter(char c, int x, int y, int fontSize, int fontType, Color color, Color* dest, int line_size, int alpha)
{
	Color *vram;
	Color *vram_ptr;
	Color *font = 0;
	Color *font_ptr;
	
	// Position ourself at the x,y position where the texts needs to be
	vram = dest + x + y * line_size;

	// Position the font pointer to the place where the letter starts we want
	if (fontSize == 0)
		font = _font0->GetSurface() + GetCharacterPositionX(c, fontSize)+1 + GetCharacterPositionY(c, fontSize, fontType) * _font0->TextureWidth;
	else if (fontSize == 1)
		font = _font1->GetSurface() + GetCharacterPositionX(c, fontSize)+1 + GetCharacterPositionY(c, fontSize, fontType) * _font1->TextureWidth;
	else if (fontSize == 2)
		font = _font2->GetSurface() + GetCharacterPositionX(c, fontSize)+1 + GetCharacterPositionY(c, fontSize, fontType) * _font2->TextureWidth;
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
					*vram_ptr = AlphaBlend(color, *vram_ptr);
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

int Graphics::Print(const char* text, int x, int y, Color color, int fontSize, int fontType, Color* dest, int line_size, int alpha)
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

int Graphics::PrintScreen(string text, int x, int y, Color color, int fontSize, int fontType)
{
	return Print(text.c_str(), x, y, color, fontSize, fontType, ScreenDriver::Instance()->GetVramBufferPointer(), ScreenDriver::Instance()->GetScreenLineSize(), 0);
}

int Graphics::PrintImage(string text, int x, int y, Image* image, Color color, int fontSize, int fontType)
{
	return Print(text.c_str(), x, y, color, fontSize, fontType, image->GetSurface(), image->TextureWidth, 0);
}

int Graphics::PrintScreen(string text, int x, int y, Color color, int fontSize)
{
	return Print(text.c_str(), x, y, color, fontSize, 0, ScreenDriver::Instance()->GetVramBufferPointer(), ScreenDriver::Instance()->GetScreenLineSize(), 0);
}

int Graphics::PrintImage(string text, int x, int y, Image* image, Color color, int fontSize)
{
	return Print(text.c_str(), x, y, color, fontSize, 0, image->GetSurface(), image->TextureWidth, 0);
}

int Graphics::PrintImage(char ch, int x, int y, Image* image, Color color, int fontSize, int fontType)
{
	return PrintLetter(ch, x, y, fontSize, fontType, color, image->GetSurface(), image->TextureWidth, 0);
}
