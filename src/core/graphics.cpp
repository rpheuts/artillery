#include <stdlib.h>
#include <malloc.h>
#include <pspdisplay.h>
#include <psputils.h>
#include <pspgu.h>
#include <string.h>
#include <png.h>

extern "C" {
#include <jpeglib.h>
#include <jerror.h>
}

#include "artillery.h"

#define FRAMEBUFFER_SIZE (PSP_LINE_SIZE*SCREEN_HEIGHT*4)
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define SLICE_SIZE 32 // change this to experiment with different page-cache sizes

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

static unsigned int __attribute__((aligned(16))) gulist[262144];
static int dispBufferNumber;
static int initialized = 0;

void Graphics::Init()
{
      dispBufferNumber = 0;
      sceDisplaySetMode(0, 480, 272);
		sceGuInit();

		// setup
		sceGuStart(GU_DIRECT,gulist);
        sceGuDrawBuffer(GU_PSM_8888, (void*)FRAMEBUFFER_SIZE, PSP_LINE_SIZE);
        sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)0, PSP_LINE_SIZE);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
        sceGuDepthBuffer((void*) (FRAMEBUFFER_SIZE*2), PSP_LINE_SIZE);
        sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
        sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
        sceGuDepthRange(0xc350, 0x2710);
        sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        sceGuEnable(GU_SCISSOR_TEST);
        sceGuAlphaFunc(GU_GREATER, 0, 0xff);
        sceGuEnable(GU_ALPHA_TEST);
        sceGuDepthFunc(GU_GEQUAL);
        sceGuEnable(GU_DEPTH_TEST);
        sceGuFrontFace(GU_CW);
        sceGuShadeModel(GU_SMOOTH);
        sceGuEnable(GU_CULL_FACE);
        sceGuEnable(GU_TEXTURE_2D);
        sceGuEnable(GU_CLIP_PLANES);
        sceGuTexMode(GU_PSM_8888, 0, 0, 0);
        sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA); //new
		  sceGuTexFilter(GU_LINEAR,GU_LINEAR);  //new 
        sceGuAmbientColor(0xffffffff);
        sceGuEnable(GU_BLEND);
        sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
        sceGuFinish();
      sceGuSync(0, 0);

        sceDisplayWaitVblankStart();
        sceGuDisplay(GU_TRUE);
        initialized = 1;

}

Image* Graphics::CreateImage(int width, int height)
{
        Image* image = (Image*) malloc(sizeof(Image));
        if (!image) return NULL;
        image->imageWidth = width;
        image->imageHeight = height;
        image->textureWidth = GetNextPower2(width);
        image->textureHeight = GetNextPower2(height);
        image->data = (Color*) memalign(16, image->textureWidth * image->textureHeight * sizeof(Color));
        if (!image->data) return NULL;
        memset(image->data, 0, image->textureWidth * image->textureHeight * sizeof(Color));
        return image;
}

Image* Graphics::ResizeImage(int width, int height, Image* source)
{
	if (source == NULL)
		return NULL;
		
	Image* destination = CreateImage(width, height);
	ResizeImage(width, height, source, destination);
	return destination;
}

void Graphics::ResizeImage(int width, int height, Image* source, Image* destination)
{
	if (source == NULL)
		return;
	Color* destinationData = &destination->data[0];
	int destinationSkipX = destination->textureWidth - width;
	Color* sourceData = &source->data[0];
	float scaleFactorX = ((float)source->imageWidth) / ((float)width);
	float scaleFactorY = ((float)source->imageHeight) / ((float)height);
	
	int x, y;
	for (y = 0; y < height; y++, destinationData += destinationSkipX) {
		for (x = 0; x < width; x++, destinationData++) {
			*destinationData = sourceData[(int)((int)(scaleFactorY * y) * source->textureWidth) + (int)(x * scaleFactorX)];
		}
	}
}

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
}

Image* Graphics::LoadPNG(const char* filename)
{
        png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height, x, y;
	int bit_depth, color_type, interlace_type;
	u32* line;
	FILE *fp;

	if ((fp = fopen(filename, "rb")) == NULL) return NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return NULL;;
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
	
	Image* image = CreateImage(width, height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	if (!image->data) {
		free(image);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	line = (u32*) malloc(width * 4);
	if (!line) {
		free(image->data);
		free(image);
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	for (y = 0; y < height; y++) {
		png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
		for (x = 0; x < width; x++) {
			u32 color = line[x];
			image->data[x + y * image->textureWidth] =  color;
		}
	}
	free(line);
	// png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);
	return image;
}

static SceUID fd = 0;

void user_read_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
   sceIoRead(fd, data, length);
}

Image* Graphics::LoadPNG(SceUID fileD)
{
   png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height, x, y;
	int bit_depth, color_type, interlace_type;
	u32* line;
	fd = fileD;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		return NULL;;
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	
	png_set_read_fn(png_ptr, (png_voidp *)fileD, user_read_data_fn); 
	png_set_error_fn(png_ptr, (png_voidp) NULL, (png_error_ptr) NULL, user_warning_fn);
	
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	
	Image* image = CreateImage(width, height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	if (!image->data) {
		free(image);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	line = (u32*) malloc(width * 4);
	if (!line) {
		free(image->data);
		free(image);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	
	for (y = 0; y < height; y++) {
		png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
		for (x = 0; x < width; x++) {
			u32 color = line[x];
			image->data[x + y * image->textureWidth] =  color;
		}
	}
	free(line);
	// png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	return image;
}

Image* Graphics::LoadJPG(const char* filename)
{
        struct jpeg_decompress_struct dinfo;
        struct jpeg_error_mgr jerr;
        dinfo.err = jpeg_std_error(&jerr);
        jpeg_create_decompress(&dinfo);
        FILE* inFile = fopen(filename, "rb");
        if (!inFile) {
                jpeg_destroy_decompress(&dinfo);
                return NULL;
        }
        jpeg_stdio_src(&dinfo, inFile);
        jpeg_read_header(&dinfo, TRUE);
        int width = dinfo.image_width;
        int height = dinfo.image_height;
        jpeg_start_decompress(&dinfo);
        Image* image = CreateImage(width, height);
        if (!image) {
                jpeg_destroy_decompress(&dinfo);
                fclose(inFile);
                return NULL;
        }
        u8* line = (u8*) malloc(width * 3);
        if (!line) {
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
                                image->data[x + image->textureWidth * y] = c | (c << 8) | (c << 16) | 0xff000000;;
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
                                image->data[x + image->textureWidth * y] = c | 0xff000000;
                        }
                }
        }
        jpeg_finish_decompress(&dinfo);
        jpeg_destroy_decompress(&dinfo);
        free(line);
        fclose(inFile);
        return image;
}

Image* Graphics::GrabScreenBuffer()
{
	Image* image = CreateImage(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	Color* destinationData = &image->data[0];
	int destinationSkipX = image->textureWidth - SCREEN_WIDTH;
	Color* sourceData = GetVramDisplayBuffer();
	int sourceSkipX = image->textureWidth - SCREEN_WIDTH;;
	int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (x = 0; x < SCREEN_WIDTH; x++, destinationData++, sourceData++) {
			*destinationData = *sourceData;
		}
	}
	return image;
}

void Graphics::FlipScreen()
{
	if (!initialized) return;
	sceGuSwapBuffers();
   dispBufferNumber ^= 1;
   
   sceGuClearColor(0);
   sceGuClear(GU_COLOR_BUFFER_BIT);
}

void Graphics::ClearScreen(Color color)
{
	/*if (!initialized) return;
	int i;
	int size = PSP_LINE_SIZE * SCREEN_HEIGHT;
	Color* data = GetVramDrawBuffer();
	for (i = 0; i < size; i++, data++) *data = color;*/
	
	sceGuStart(GU_DIRECT,gulist);
   sceGuClearColor(0);
   sceGuClear(GU_COLOR_BUFFER_BIT);
   sceGuFinish();
   sceGuSync(0,0); 
}

void Graphics::BlitImageToImage(int sx, int sy, int width, int height, Image* source, int dx, int dy, Image* destination)
{
	Color* destinationData = &destination->data[destination->textureWidth * dy + dx];
	int destinationSkipX = destination->textureWidth - width;
	Color* sourceData = &source->data[source->textureWidth * sy + sx];
	int sourceSkipX = source->textureWidth - width;
	int x, y;
	for (y = 0; y < height; y++, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (x = 0; x < width; x++, destinationData++, sourceData++) {
			*destinationData = *sourceData;
		}
	}
}

void Graphics::BlitImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy)
{
	if (!initialized) return;
	Color* vram = GetVramDrawBuffer();
	sceKernelDcacheWritebackInvalidateAll();
	sceGuStart(GU_DIRECT,gulist);
	sceGuTexMode(GU_PSM_8888,0,0,0);
	sceGuCopyImage(GU_PSM_8888, sx, sy, width, height, source->textureWidth, source->data, dx, dy, PSP_LINE_SIZE, vram);
	sceGuFinish();
	sceGuSync(0,0);
}

void Graphics::BlitAlphaImageToImage(int sx, int sy, int width, int height, Image* source, int dx, int dy, Image* destination)
{
	Color* destinationData = &destination->data[destination->textureWidth * dy + dx];
	int destinationSkipX = destination->textureWidth - width;
	Color* sourceData = &source->data[source->textureWidth * sy + sx];
	int sourceSkipX = source->textureWidth - width;
	int x, y;
	for (y = 0; y < height; y++, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (x = 0; x < width; x++, destinationData++, sourceData++) {
			Color color = *sourceData;
			if (!IS_ALPHA(color)) *destinationData = AlphaBlend(color, *destinationData);
		}
	}
}


void Graphics::SavePNG(const char* filename, Color* data, int width, int height, int lineSize, int saveAlpha)
{
        png_structp png_ptr;
        png_infop info_ptr;
        FILE* fp;
        int i, x, y;
        u8* line;
        
        if ((fp = fopen(filename, "wb")) == NULL) return;
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) return;
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
                png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
                return;
        }
        png_init_io(png_ptr, fp);
        png_set_IHDR(png_ptr, info_ptr, width, height, 8,
                saveAlpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        png_write_info(png_ptr, info_ptr);
        line = (u8*) malloc(width * (saveAlpha ? 4 : 3));
        for (y = 0; y < height; y++) {
                for (i = 0, x = 0; x < width; x++) {
                        Color color = data[x + y * lineSize];
                        u8 r = color & 0xff; 
                        u8 g = (color >> 8) & 0xff;
                        u8 b = (color >> 16) & 0xff;
                        u8 a = saveAlpha ? (color >> 24) & 0xff : 0xff;
                        line[i++] = r;
                        line[i++] = g;
                        line[i++] = b;
                        if (saveAlpha) line[i++] = a;
                }
                png_write_row(png_ptr, line);
        }
        free(line);
        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
}

void Graphics::BlitAlphaImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy)
{
        if (!initialized) return;

        sceKernelDcacheWritebackInvalidateAll();
        sceGuStart(GU_DIRECT, gulist);
        sceGuTexMode(GU_PSM_8888,0,0,0);
        sceGuTexImage(0, source->textureWidth, source->textureHeight, source->textureWidth, (void*) source->data);
        sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
		  sceGuTexFilter(GU_LINEAR,GU_LINEAR); 
        float u = 1.0f / ((float)source->textureWidth);
        float v = 1.0f / ((float)source->textureHeight);
        sceGuTexScale(u, v);
        
        int j = 0;
        while (j < width) {
                Vertex* vertices = (Vertex*) sceGuGetMemory(2 * sizeof(Vertex));
                int sliceWidth = 64;
                if (j + sliceWidth > width) sliceWidth = width - j;
                vertices[0].u = sx + j;
                vertices[0].v = sy;
                vertices[0].x = dx + j;
                vertices[0].y = dy;
                vertices[0].z = 0;
                vertices[1].u = sx + j + sliceWidth;
                vertices[1].v = sy + height;
                vertices[1].x = dx + j + sliceWidth;
                vertices[1].y = dy + height;
                vertices[1].z = 0;
                sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
                j += sliceWidth;
        }
        
        sceGuFinish();
        sceGuSync(0, 0);
}

void Graphics::SaveScreenBuffer(const char* filename)
{
	SavePNG(filename, GetVramDisplayBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, PSP_LINE_SIZE, 0);
}

void Graphics::FreeImage(Image* image)
{
	if (image)
	{
		if (image->data)
			free(image->data);
		free(image);
	}
}

void Graphics::ClearImage(Color color, Image* image)
{
	int i;
	int size = image->textureWidth * image->textureHeight;
	Color* data = image->data;
	for (i = 0; i < size; i++, data++) *data = color;
}

void Graphics::DisableGraphics()
{
	initialized = 0;
}

void Graphics::WaitVblankStart()
{
	sceDisplayWaitVblankStart();
}

//---------------------------------------------------------------------------------------------------------------------
// Private Functions

int Graphics::GetNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}

Color* Graphics::GetVramDrawBuffer()
{
	Color* vram = (Color*) g_vram_base;
	if (dispBufferNumber == 0) vram += FRAMEBUFFER_SIZE / sizeof(Color);
	return vram;
}

Color* Graphics::GetVramDisplayBuffer()
{
	Color* vram = (Color*) g_vram_base;
	if (dispBufferNumber == 1) vram += FRAMEBUFFER_SIZE / sizeof(Color);
	return vram;
}

unsigned long Graphics::AlphaBlend(unsigned long front, unsigned long back)
{
   unsigned short alpha = GET_ALPHA(front);
   unsigned short beta = 255 - alpha;

   return MK8888(
	   ((GET_BLUE(front) * alpha) + (GET_BLUE(back) * beta)) / 255,
       ((GET_GREEN(front) * alpha) + (GET_GREEN(back) * beta)) / 255,
       ((GET_RED(front) * alpha) + (GET_RED(back) * beta)) / 255,
	   (GET_ALPHA(back)));
}
