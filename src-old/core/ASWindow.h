#ifndef ASWINDOW_H_
#define ASWINDOW_H_

#include "ASIncludes.h"

/** This class represents a window used in the WindowManager and
	throughout the application. Most of the public functions are
	used by the WindowManager to update the application running
	inside the window.
 */
class ASWindow
{
	public:
		ASWindow();
		
		/** Used by the WindowManager to give the application running
			inside the window the opportunity to handle the controller input.
			Note: This will actually call the same function from the WindowApp
			class.
		 */
		void HandleController(int mouseX, int mouseY, int btn);
		
		/** Used by the WindowManager to give the application running
			inside the window the opportunity to free resources and exit.
			Note: This will actually call the same function from the WindowApp
			class.
		 */
		void Destroy();
		
		/** Used by the WindowManager to give the application running
			inside the window the opportunity to update the internal screen-
			buffer. 
			Note: This will actually call the same function from the WindowApp
			class.
		 */
		void Render();
		
		/** Used by the WindowManager to give the application running
			inside the window the opportunity to take over the full screen.
			Note: This will actually call the same function from the WindowApp
			class.
		 */
		void RenderFullscreen();
	
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
		int OnTop;
		ASWindow* Next;
		ASWindow* Previous;
		ASImage* Topbar;
		ASWindowApplication* Application;
};
#endif
