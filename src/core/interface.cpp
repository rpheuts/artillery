#include "artillery.h"

bool Interface::IsWithinWindow(int x, int y, Window* window)
{
	if (!window->Fullscreen)
		return IsWithinArea(x, y, window->X, window->Y, window->Width, window->Height);
	else
		return true;
}

bool Interface::IsWithinWindowContent(int x, int y, Window* window)
{
	if (!window->Fullscreen)
		return IsWithinArea(x, y, window->ContentX, window->ContentY, window->Screen->imageWidth, window->Screen->imageHeight);
	else
		return true;
}

bool Interface::IsWithinImage(int x, int y, int tx, int ty, Image* image)
{
	return IsWithinArea(x, y, tx, ty, image->imageWidth, image->imageHeight);
}
bool Interface::IsWithinArea(int x, int y, int tx, int ty, int tWidth, int tHeight)
{
	if (x > tx)
	{
		if (y > ty)
		{
			if (x < (tx + tWidth))
			{
				if (y < (ty + tHeight))
					return true;
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}
