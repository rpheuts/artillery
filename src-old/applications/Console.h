#ifndef ASCONSOLE_H_
#define ASCONSOLE_H_

#include "ASIncludes.h"

/** Provides a simple debug window in the form of a drop down console.
 */
class Console : public ASWindowApplication
{
	public:
		Console();
		~Console();
		
		/** Controller handler function, used mainly for scrolling.
		 */
		void HandleController(int mouseX, int mouseY, int btn);
		
		/** Callback function for the interface manager
		 */
		void HandleInputEvent(string EventID, string arg);
		
		/** Cleans up the resources used.
		 */
		void Destroy();
		
		/** Main render function, used to update the internal screen buffer.
		 */
		void Render();
		
		/** Used when the console should be rendered fullscreen (not used).
		 */
		void RenderFullscreen();
		
	private:
		int _cursorY;
};
#endif
