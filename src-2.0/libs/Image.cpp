#include "Includes.h"
#include "malloc.h"
#include <pspgu.h>

Image::Image(int width, int height)
{
	if (width > 0 && height > 0)
	{
		Width = width;
		Height = height;
		
		// Set the texture width, needed for blitting
		TextureWidth = GetNextPower2(Width);
		TextureHeight = GetNextPower2(Height);
		
		// Init the surface
		_surface = (Color*) memalign(16, TextureWidth * TextureHeight * sizeof(Color));
		if (!_surface)
			return;
		
		memset(_surface, 0, TextureWidth * TextureHeight * sizeof(Color));
	}
}

Image::~Image()
{
	free(_surface);
}

void Image::Clear(Color color)
{
	int i;
	int size = this->TextureWidth * this->TextureHeight;
	Color* data = this->_surface;
	for (i = size-1; i > 0; i--, data++) 
		*data = color;
	data = 0;
}

Color* Image::GetSurface()
{
	return _surface;
}

void Image::BlitToScreen(int targetX, int targetY)
{
	BlitToScreen(this->Width, this->Height, targetX, targetY);
}

void Image::BlitToScreen(int width, int height, int targetX, int targetY)
{
	BlitToScreen( width, height, 0, 0, targetX, targetY);
}

void Image::BlitToScreen(int width, int height, int sourceX, int sourceY, int targetX, int targetY)
{
	ScreenDriver* screenDriver = ScreenDriver::Instance();
	sceKernelDcacheWritebackInvalidateAll();
	sceGuStart(GU_DIRECT, ScreenDriver::Instance()->GetGUListPointer());
	sceGuTexMode(GU_PSM_8888,0,0,0);
	sceGuCopyImage(GU_PSM_8888, sourceX, sourceY, width, height, this->TextureWidth, this->_surface, targetX, targetY, screenDriver->GetScreenLineSize(), screenDriver->GetVramBufferPointer());
	sceGuFinish();
	sceGuSync(0,0);
}

void Image::AlphaBlitToScreen(int targetX, int targetY)
{
	AlphaBlitToScreen(this->Width, this->Height, targetX, targetY);
}

void Image::AlphaBlitToScreen(int width, int height, int targetX, int targetY)
{
	AlphaBlitToScreen(width, height, 0, 0, targetX, targetY);
}

void Image::AlphaBlitToScreen(int width, int height, int sourceX, int sourceY, int targetX, int targetY)
{
	sceKernelDcacheWritebackInvalidateAll();
	sceGuStart(GU_DIRECT, ScreenDriver::Instance()->GetGUListPointer());
	sceGuTexMode(GU_PSM_8888,0,0,0);
	sceGuTexImage(0, this->TextureWidth, this->TextureHeight, this->TextureWidth, (void*) this->_surface);
	sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR); 
	float u = 1.0f / ((float)this->TextureWidth);
	float v = 1.0f / ((float)this->TextureHeight);
	sceGuTexScale(u, v);
        
	int j = 0;
	while (j < width) {
		Vertex* vertices = (Vertex*) sceGuGetMemory(2 * sizeof(Vertex));
		int sliceWidth = 64;
		if (j + sliceWidth > width) sliceWidth = width - j;
		vertices[0].u = sourceX + j;
		vertices[0].v = sourceY;
		vertices[0].x = targetX + j;
		vertices[0].y = targetY;
		vertices[0].z = 0;
		vertices[1].u = sourceX + j + sliceWidth;
		vertices[1].v = sourceY + height;
		vertices[1].x = targetX + j + sliceWidth;
		vertices[1].y = targetY + height;
		vertices[1].z = 0;
		sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
		j += sliceWidth;
	}
        
	sceGuFinish();
	sceGuSync(0, 0);
}

void Image::BlitToImage(int targetX, int targetY, Image* target)
{
	BlitToImage(this->Width, this->Height, targetX, targetY, target);
}

void Image::BlitToImage(int width, int height, int targetX, int targetY, Image* target)
{
	BlitToImage(width, height, 0, 0, targetX, targetY, target);
}

void Image::BlitToImage(int width, int height, int sourceX, int sourceY, int targetX, int targetY, Image* target)
{
	/*sceKernelDcacheWritebackInvalidateAll();
	sceGuStart(GU_DIRECT, ScreenDriver::Instance()->GetGUListPointer());
	sceGuTexMode(GU_PSM_8888,0,0,0);
	sceGuCopyImage(GU_PSM_8888, sourceX, sourceY, width, height, this->TextureWidth, this->_surface, targetX, targetY, target->TextureWidth, target->GetSurface());
	sceGuFinish();
	sceGuSync(0,0);*/
	
	Color* destinationData = &target->GetSurface()[target->TextureWidth * targetY + targetX];
	int destinationSkipX = target->TextureWidth - width;
	Color* sourceData = &this->GetSurface()[this->TextureWidth * sourceY + sourceX];
	int sourceSkipX = this->TextureWidth - width;
	int x, y;
	for (y = 0; y < height; y++, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (x = 0; x < width; x++, destinationData++, sourceData++) {
			*destinationData = *sourceData;
		}
	}
}

void Image::AlphaBlitToImage(int targetX, int targetY, Image* target)
{
	AlphaBlitToImage(this->Width, this->Height, targetX, targetY, target);
}

void Image::AlphaBlitToImage(int width, int height, int targetX, int targetY, Image* target)
{
	AlphaBlitToImage(width, height, 0, 0, targetX, targetY, target);
}

void Image::AlphaBlitToImage(int width, int height, int sourceX, int sourceY, int targetX, int targetY, Image* target)
{
	Color* destinationData = &target->GetSurface()[target->TextureWidth * targetY + targetX];
	int destinationSkipX = target->TextureWidth - width;
	Color* sourceData = &this->_surface[this->TextureWidth * sourceY + sourceX];
	int sourceSkipX = this->TextureWidth - width;
	int x, y;
	for (y = height-1; y >= 0; y--, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (x = width-1; x >= 0; x--, destinationData++, sourceData++) {
			Color color = *sourceData;
			if (!IS_ALPHA(color)) *destinationData = AlphaBlend(color, *destinationData);
		}
	}
}

int Image::Print(string text, int x, int y, Color color, int fontSize, int fontType)
{
	return Graphics::Instance()->PrintImage(text, x, y, this, color, fontSize, fontType);
}

int Image::Print(string text, int x, int y, Color color, int fontSize)
{
	return Graphics::Instance()->PrintImage(text, x, y, this, color, fontSize);
}

int Image::Print(char ch, int x, int y, Color color, int fontSize, int fontType)
{
	return Graphics::Instance()->PrintImage(ch, x, y, this, color, fontSize, fontType);
}

void Image::ApplyEffect(ImageEffect effect)
{
	ApplyEffect(effect, 0);
}
	
void Image::ApplyEffect(ImageEffect effect, int amount)
{
	switch (effect)
	{
		case ImageBlur:
			ApplyBlur();
			break;
		case ImageFade:
			ApplyFade(amount, false);
			break;
		case ImageAlpha:
			ApplyFade(amount, true);
			break;
		default:
			break;
	}
}

void Image::ApplyFade(int fadeAmount, bool set)
{
	if (!set || (fadeAmount > 0 && fadeAmount < 256))
	{
		Color* data = this->_surface;
		// Loop through the horizontal lines
		for (int y = this->Height - 1; y >= 0; y--)
		{
			// Loop through the pixels on each line
			for (int x = this->Width - 1; x >=0; x--)
			{
				int position = (y*this->TextureWidth) + x;
				if (set)
					data[position] = MK8888(GET_BLUE(data[position]), GET_GREEN(data[position]),
							GET_RED(data[position]),fadeAmount);
				else
				{
					short alpha = GET_ALPHA(data[position]) + fadeAmount;
					alpha = alpha > 0 ? alpha : 0; 
					data[position] = MK8888(GET_BLUE(data[position]), GET_GREEN(data[position]),
							GET_RED(data[position]), alpha);
				}
			}
		}
	}
}

void Image::ApplyBlur()
{
	int dwLineWidth = this->TextureWidth, bMatrixSum = 0;
	int dwImageSize = this->TextureWidth * this->TextureHeight;
	int dwAverageRed = 0, dwAverageGreen = 0, dwAverageBlue = 0;
	short dwRed = 0, dwGreen = 0, dwBlue = 0;
	Color* data = this->_surface;
	
	// Blending matrix, indicates how much each surrounding pixel influences the blend
	int bBlurBlendingMatrix[9] = { 1, 2, 1,
					2, 4, 2,
					1, 2, 1 };
	int nPosRange[9] = { - (dwLineWidth-1) ,
				- dwLineWidth ,
				- (dwLineWidth+1),
				- 1 ,
				0 ,
				+ 1,
				+ (dwLineWidth-1) ,
				+ dwLineWidth ,
				+ (dwLineWidth+1) };
	
	for( int iSum = 0 ; iSum<9 ; iSum++ )
		bMatrixSum += bBlurBlendingMatrix[iSum];
	
	// Loop through the horizontal lines
	for (int y = this->Height - 1; y >= 0; y--)
	{
		// Loop through the pixels on each line
		for (int x = this->Width - 1; x >=0; x--)
		{
			int position = (y*this->TextureWidth) + x;
			// Create array of pixel positions
			long nPosNewRange[9]= { (long)(position+nPosRange[0]),
				(long)(position+nPosRange[1]),
				(long)(position+nPosRange[2]),
				(long)(position+nPosRange[3]),
				(long)(position+nPosRange[4]),
				(long)(position+nPosRange[5]),
				(long)(position+nPosRange[6]),
				(long)(position+nPosRange[7]),
				(long)(position+nPosRange[8]) };
			
			// Check point validity : ensure points are within the image
			for( int iPos = 0 ; iPos < 9 ; iPos++ )
			{
				nPosNewRange[iPos] = nPosNewRange[iPos] < 0 ? 0 :
						nPosNewRange[iPos] > dwImageSize ? position : nPosNewRange[iPos];
			}
			
			// Get Color values
			Color bColorValue[9] = { data[nPosNewRange[0]],
				data[nPosNewRange[1]],
				data[nPosNewRange[2]],
				data[nPosNewRange[3]],
				data[nPosNewRange[4]],
				data[nPosNewRange[5]],
				data[nPosNewRange[6]],
				data[nPosNewRange[7]],
				data[nPosNewRange[8]] };
				
			dwAverageRed = 0;
			dwAverageGreen = 0;
			dwAverageBlue = 0;	
				
			for (int iPos = 0; iPos < 9; iPos++)
			{
				dwAverageRed += GET_RED(bColorValue[iPos])*bBlurBlendingMatrix[iPos];
				dwAverageGreen += GET_GREEN(bColorValue[iPos])*bBlurBlendingMatrix[iPos];
				dwAverageBlue += GET_BLUE(bColorValue[iPos])*bBlurBlendingMatrix[iPos];
			}
			
			dwRed = (short)(dwAverageRed / bMatrixSum);
			dwGreen = (short)(dwAverageGreen / bMatrixSum);
			dwBlue = (short)(dwAverageBlue / bMatrixSum);
			
			data[position] = MK8888(dwBlue, dwGreen, dwRed, GET_ALPHA(data[position]));
		}
	}
	
}

// Used for calculating the actual texture width (power of 2) for blitting
int Image::GetNextPower2(int number)
{
	int b = number;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * number) b >>= 1;
	return b;
}
