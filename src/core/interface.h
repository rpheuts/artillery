#include "artillery.h"

class Interface{
	public:
		static bool IsWithinWindow(int x, int y, Window* window);
		static bool IsWithinWindowContent(int x, int y, Window* window);
		static bool IsWithinImage(int x, int y, int tx, int ty, Image* image);
		static bool IsWithinArea(int x, int y, int tx, int ty, int tWidth, int tHeight);
};
