#ifndef INTERFACE_H
#define INTERFACE_H

#include "ASIncludes.h"

class ASInterface{
	public:
		static inline bool ASInterface::IsWithinWindow(int x, int y, ASWindow* window)
		{
			if (!window->Fullscreen)
				return IsWithinArea(x, y, window->X, window->Y, window->Width, window->Height);
			else
		return true;
		}

		static inline bool ASInterface::IsWithinWindowContent(int x, int y, ASWindow* window)
		{
			if (!window->Fullscreen)
				return IsWithinArea(x, y, window->ContentX, window->ContentY, window->Application->Screen->Width, window->Application->Screen->Height);
			else
				return true;
		}

		static inline bool ASInterface::IsWithinImage(int x, int y, int tx, int ty, ASImage* image)
		{
			return IsWithinArea(x, y, tx, ty, image->Width, image->Height);
		}

		static inline bool ASInterface::IsWithinArea(int x, int y, int tx, int ty, int tWidth, int tHeight)
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
};
#endif
