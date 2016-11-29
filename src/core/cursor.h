#include <pspctrl.h>
#include "artillery.h"

class CursorInfo{
	public:
		int CursorSpeed;
		int X;
		int Y;
		Image* Display;
};

class Cursor{
	public:
		static void Init();
		static void Destroy();
		static void Handle(SceCtrlData* pad);
		
	private:
		static CursorInfo* cursor;
};
