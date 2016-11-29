#include "artillery.h"

class Window
{
	public:
		int X;
		int Y;
		int Width;
		int Height;
		int Borders;
		int Enabled;
		int ContentX;
		int ContentY;
		int Fullscreen;
		int Transparent;
		Window* Next;
		Window* Previous;
		Image* Topbar;
		Image* Screen;
		
	public:
		void (*Handle)(int mouseX, int mouseY, int btn);
		void (*Destroy)(void);
		void (*Render)(void);
		void (*RenderFullscreen)(void);
		
};
