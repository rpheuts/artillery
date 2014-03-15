#ifndef ASWINDOWAPP_H_
#define ASWINDOWAPP_H_

#include "ASIncludes.h"

/** Helper class to support the Window class. 
 */
class ASWindowApplication
{
	public:
		ASWindowApplication(int width, int height);
		virtual ~ASWindowApplication();
		
		/** Used by the WindowManager to give the application running
		inside the window the opportunity to handle the controller input.
		Note: This will actually call the same function from the WindowApp
		class.
		 */
		virtual void HandleController(int mouseX, int mouseY, int btn);
		
		/** This is used by the MSBInterfaceManager as a callback to handle
			input events.
		 */
		virtual void HandleInputEvent(string EventID, string arg);
		
		/** Used by the WindowManager to give the application running
		inside the window the opportunity to free resources and exit.
		Note: This will actually call the same function from the WindowApp
		class.
		 */
		virtual void Destroy();
		
		/** Used by the WindowManager to give the application running
		inside the window the opportunity to update the internal screen-
		buffer. 
		Note: This will actually call the same function from the WindowApp
		class.
		 */
		virtual void Render();
		
		/** Used by the WindowManager to give the application running
		inside the window the opportunity to take over the full screen.
		Note: This will actually call the same function from the WindowApp
		class.
		 */
		virtual void RenderFullscreen();
		
	public:
		ASImage* Screen;
		int Width;
		int Height;
		int X;
		int Y;
};
#endif
