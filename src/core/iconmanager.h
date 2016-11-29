#include "artillery.h"

class Icon
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
		Image* Display;
		Image* DisplaySel;
		Icon* Next;
		
	public:
		Icon(int x, int y, Image* image, Image* imageSel, void (*ptf)(void), string name);
		Icon(int x, int y, Image* image, Image* imageSel, string path, string name);
		void RunInternal();
};

class IconManager{
	public:
		static Icon* CreateIcon(int x, int y, Image* image, Image* imageSel, void (*ptf)(void), string name);
		static void Render();
		static void Init();
		static void Destroy();
		static void Handle();
		
	private:
		static void RegisterIcon(Icon* icon);
		static void SetIconPosition(Icon* icon, int x, int y);
		static void HandleIconDrag();
		static void HandleIconClick();
};
