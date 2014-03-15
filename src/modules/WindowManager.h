#ifndef WINDOWMANAGER_H_
#define WINDOWMANAGER_H_

#include "Includes.h"

/** This class handles the creation, destroying and managing of windows.
	Some analog input is processed here to support window dragging
	and focussing.
 */
class WindowManager : public Module
{
	public:
		/** Sets input focus to a window and sets it on top of the other windows.
		 */
		void Focus(WindowApplication* window);
		
		/** Starts the Window destroying process. The application running inside the window
		will be given the opportunity to exit gracefully.
		 */
		void DestroyWindow(WindowApplication* window);
		
		/** Should be called from the main loop (Core) to update the user input and distribute
		it to the current windows.
		 */
		virtual void Tick();
		
		/** Sets the position of the specified window to the specified screen coordinates.
		Windows are autmatically clipped when they extend the physical screen size.
		 */
		void WindowPosition(WindowApplication* window, int x, int y);
		
		/** Adds a window to the internal window list
		 */
		void RegisterWindow(WindowApplication* window);
		
		Image* RenderWindowToImage(WindowApplication* window);
		
	public:
		int FullScreenEnabled;
		
	protected:
		Image* InitTopbar(int width, string text);
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
		
		/** Renders a window and its content to the screen
		 */
		void RenderWindow(WindowApplication* window);
		
		void Render();
		
		WindowManager();
		~WindowManager();
		
	protected:
		list<WindowApplication*> _windows;
		Graphics* _graphics;
		Controller* _controller;
		Image* _shadowHorizontal;
		Image* _shadowVerticalRight;
		Image* _shadowVerticalLeft;
		Image* _shadowCornerRight;
		Image* _shadowCornerLeft;
		Image* _buttonClose;
		Image* _topbarLeft;
		Image* _topbarRight;
		Image* _topbarCenter;
		Image* _topbarScaleLeft;
		Image* _topbarScaleRight;
		int _titlebarTextOffsetX;
		int _titlebarTextOffsetY;
		
	private:
		int _createdLastX;
		int _createdLastY;
		int _isBeingDragged;
		int _isActive;
};
#endif
