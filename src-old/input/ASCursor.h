#ifndef ASCURSOR_H
#define ASCURSOR_H


#include <pspctrl.h>
#include "ASIncludes.h"

class ASCursorInfo{
	public:
		int CursorSpeed;
		int X;
		int Y;
		ASImage* Display;
		ASImage* DisplayPress;
};

class ASCursor{
	public:
		ASCursor();
		~ASCursor();
		void Handle(SceCtrlData* pad);
		void SetX(int xpos);
		void SetY(int ypos);
		int ToggleDigital();
		
	public:
		int Digital;
		
	private:
		ASCursorInfo* _cursor;
		ASGraphics* _graphics;
		ASController* _controller;
};

#endif
