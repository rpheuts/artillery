#ifndef ASICON_H_
#define ASICON_H_


#include "ASIncludes.h"

class ASIcon
{
	public:
		int X;
		int Y;
		int Width;
		int Height;
		int Selected;
		int Dragged;
		int InternalFunction;
		void (*Run)(void);
		string Path;
		string Name;
		ASImage* Display;
		ASImage* DisplaySel;
		ASIcon* Next;
		
	public:
		ASIcon(int x, int y, ASImage* image, ASImage* imageSel, void (*ptf)(void), string name);
		ASIcon(int x, int y, ASImage* image, ASImage* imageSel, string path, string name);
		void RunInternal();
};

class ASIconManager{
	public:
		ASIconManager();
		~ASIconManager();
		ASIcon* CreateIcon(int x, int y, ASImage* image, ASImage* imageSel, void (*ptf)(void), string name);
		void Render();
		void Handle();
		
	private:
		void RegisterIcon(ASIcon* icon);
		void SetIconPosition(ASIcon* icon, int x, int y);
		void HandleIconDrag();
		void HandleIconClick();
		
	private:
		int _gridSize;
		int _isDraging;
		ASIcon* _listStart;
		
};

#endif
