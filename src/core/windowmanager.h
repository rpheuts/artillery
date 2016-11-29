#ifndef WINDOWMANAGER_H_
#define WINDOWMANAGER_H_

#include "artillery.h"

class WindowManager{
	public:
	  	static Window* CreateWindow(int width, int height, void (*handleController)(int mouseX, int mouseY, int btn), void (*render)(void), void (*handleDestroy)(void), char * name, int transparent, int borders);
		static Window* CreateWindow(int width, int height, void (*handleController)(int mouseX, int mouseY, int btn), void (*render)(void), void (*handleDestroy)(void), char * name, int transparent);
		static Window* CreateWindow(int width, int height, void (*handleController)(int mouseX, int mouseY, int btn), void (*render)(void), void (*handleDestroy)(void), void (*handleFullScreen)(void), char * name, int fullscreen);
		static void Render();
		static void Focus(Window* window);
		static void DestroyWindow(Window* window);
		static void Init();
		static void Destroy();
		static void Handle();
		static void WindowPosition(Window* window, int x, int y);
		static int FullScreenEnabled;
		
	protected:
		static void HandleFocus();
		static void HandleDrag();
		static void HandleController();
		static void RegisterWindow(Window* window);
		static Image* InitTopbar(int width, char * text);
};
#endif
