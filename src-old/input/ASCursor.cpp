#include <pspctrl.h>
#include "ASIncludes.h"

ASCursor::ASCursor()
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	
	_graphics =  ASGraphics::Instance();
	_controller = ASController::Instance(); 
	_cursor = new ASCursorInfo();
	_cursor->X = 480/2;
	_cursor->Y = 280/2;
	ASCore::MainConfig->GetIntValue("cursorspeed", _cursor->CursorSpeed);
	_cursor->Display = _graphics->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_cursor"));
	_cursor->DisplayPress = _graphics->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_cursorpress"));
	Digital = 0;
	//Core::Dbg->Log(Init, "Cursor Loaded...");
}

ASCursor::~ASCursor()
{
	delete (_cursor->Display);
	delete (_cursor);
}

void ASCursor::Handle(SceCtrlData* pad)
{
	if (Digital == 0)
	{
		_controller->PreviousAnalogX = _cursor->X;
		_controller->PreviousAnalogY = _cursor->Y;
		
		bool change = false;
		// Update position
		if (pad->Lx > 145){ _cursor->X += (pad->Lx - 145) / _cursor->CursorSpeed; change = true;}
		if (pad->Lx < 93) { _cursor->X -= (93 - pad->Lx) / _cursor->CursorSpeed; change = true;}
		if (pad->Ly > 160){ _cursor->Y += (pad->Ly - 160) / _cursor->CursorSpeed; change = true;}
		if (pad->Ly < 100){ _cursor->Y -= (100 - pad->Ly) / _cursor->CursorSpeed; change = true;}
		if (!change)
		{
			_graphics->BlitAlphaImageToScreen(0, 0, _cursor->Display->Width, _cursor->Display->Height, _cursor->Display, _cursor->X, _cursor->Y);
			return;
		}	
		// Check if cursor is still on the screen
		if (_cursor->X < 0) _cursor->X=0;
		if (_cursor->X > 480) _cursor->X = 480;
		if (_cursor->Y < 0) _cursor->Y=0;
		if (_cursor->Y > 272) _cursor->Y = 272;
		
		_graphics->BlitAlphaImageToScreen(0, 0, _cursor->Display->Width, _cursor->Display->Height, _cursor->Display, _cursor->X, _cursor->Y);

		_controller->AnalogX = _cursor->X;
		_controller->AnalogY = _cursor->Y;
	}
	else
		_graphics->BlitAlphaImageToScreen(0, 0, _cursor->Display->Width, _cursor->Display->Height, _cursor->Display, _cursor->X, _cursor->Y);
}

void ASCursor::SetX(int xpos)
{
	_cursor->X = xpos;
	_controller->AnalogX = xpos;
}

void ASCursor::SetY(int ypos)
{
	_cursor->Y = ypos;
	_controller->AnalogY = ypos;
}

int ASCursor::ToggleDigital()
{
	return Digital ^= 1;
}
