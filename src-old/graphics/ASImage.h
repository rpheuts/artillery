/*
# PSP Framework 				
# ------------------------------------------------------------------
# Author: R.P.Heuts
# Date: 23-05-2006
# Version: 0.1
# 
# Class: TLImage
# Desciption:
# Represents an Image entity that can be blitted to the screen. It is
# possible to blit to the image and perform several transformations to
# it.
*/

#ifndef ASIMAGE_H
#define ASIMAGE_H

#include "ASIncludes.h"

class ASImage{
	public:
		ASImage(int width, int height);
		~ASImage();
		void Clear(Color color);
		
	private:
		int GetNextPower2(int width);
	
	public:
		int Width;
		int Height;
		int TextureWidth;
		int TextureHeight;
		Color* Surface;
};

#endif
