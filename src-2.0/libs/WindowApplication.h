#ifndef WINDOWAPPLICATION_H_
#define WINDOWAPPLICATION_H_

#include "Includes.h"

class WindowSettings
{
	public:
		int Borders;
		int Enabled;
		int Fullscreen;
		int Transparent;
		int OnTop;	
};

/** This class represents a window used in the WindowManager and
	throughout the application. Most of the public functions are
	used by the WindowManager to update the application running
	inside the window.
 */
class WindowApplication
{
	public:
		WindowApplication(string name, int width, int height);
		
		WindowApplication(string name, int width, int height, WindowSettings* settings);
		
		WindowApplication(string name, int width, int height, int borders, int enabled, int fullscreen, int transparent, int onTop);
		
		virtual ~WindowApplication();
		
		/** Used by the WindowManager to give the application running
		inside the window the opportunity to handle the controller input.
		 */
		virtual void HandleController(int mouseX, int mouseY);
		
		/** Used by the WindowManager to give the application running
		inside the window the opportunity to free resources and exit.
		 */
		virtual void DestroyHandler();
		
		/** Used by the WindowManager to give the application running
		inside the window the opportunity to update the internal screen-
		buffer. 
		 */
		virtual void Render();
		
		/** Used by the WindowManager to give the application running
		inside the window the opportunity to take over the full screen.
		 */
		virtual void RenderFullscreen();
		
		void Focus();
		
		void Destroy();
		
		void Position(int x, int y);
		
		void Register();
		
		Image* RenderToImage();
	
	public:
		int X;
		int Y;
		int ContentX;
		int ContentY;
		int Width;
		int Height;
		string Name;
		Image* Topbar;
		Image* Screen;
		WindowSettings* Settings;
};
#endif
