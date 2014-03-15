#ifndef ASWINDOWMANAGER_H_
#define ASWINDOWMANAGER_H_

#include "ASIncludes.h"

/** This class handles the creation, destroying and managing of windows.
	Some analog input is processed here to support window dragging
	and focussing.
 */
class ASWindowManager{
	public:
		static ASWindowManager* Instance();
		
		/** Creates a window with the specified parameters and returns an instance of Window.
		 */
		ASWindow* CreateWindow(ASWindowApplication* application, string name, int transparent, int fullscreen, int borders);
		
		/** Should be called from the main loop (Core) to update all windows.
		 */
		void Render();
		
		/** Sets input focus to a window and sets it on top of the other windows.
		 */
		void Focus(ASWindow* window);
		
		/** Starts the Window destroying process. The application running inside the window
			will be given the opportunity to exit gracefully.
		 */
		void DestroyWindow(ASWindow* window);
		
		/** Should be called from the main loop (Core) to update the user input and distribute
			it to the current windows.
		 */
		void Handle();
		
		/** Sets the position of the specified window to the specified screen coordinates.
			Windows are autmatically clipped when they extend the physical screen size.
		 */
		void WindowPosition(ASWindow* window, int x, int y);
		
	public:
		int FullScreenEnabled;
		
	private:
		ASWindowManager();
		
		~ASWindowManager();
		
		ASImage* InitTopbar(int width, string text);
		/** Checks the user input to see if another window needs focus.
		 */
		void HandleFocus();
		
		/** Checks the user input to see if a window is being dragged.
		 */
		void HandleDrag();
		
		/** Handles user input for window focussing / dragging and will tranfer control
			input to the active window.
		 */
		void HandleController();
		
		/** Adds a window to the internal window list
		 */
		void RegisterWindow(ASWindow* window);
		
		/** Renders a window and its content to the screen
		 */
		void RenderWindow(ASWindow* t);
		
	private:
		static ASWindowManager* _instance;
		static ASWindow* ListStart;
		static ASWindow* ListEnd;
		ASGraphics* _graphics;
		ASController* _controller;
		ASImage* InitTopbar(int width, char * text);
		int _createdLastX;
		int _createdLastY;
		int _isBeingDragged;
		int _isActive;
		ASImage* _shadowHorizontal;
		ASImage* _shadowVerticalRight;
		ASImage* _shadowVerticalLeft;
		ASImage* _shadowCornerRight;
		ASImage* _shadowCornerLeft;
		ASImage* _buttonClose;
		ASImage* _topbarLeft;
		ASImage* _topbarRight;
		ASImage* _topbarCenter;
		ASImage* _topbarScaleLeft;
		ASImage* _topbarScaleRight;
		int _titlebarTextOffsetX;
		int _titlebarTextOffsetY;
};
#endif
