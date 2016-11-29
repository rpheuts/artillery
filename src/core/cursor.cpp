#include <pspctrl.h>
#include <malloc.h>
#include "artillery.h"

CursorInfo* Cursor::cursor;

void Cursor::Init()
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
	
	Config* cfg = new Config("ms0:/MbShell/config/main.cfg");
	
	cursor = new CursorInfo();
	cursor->X = 480/2;
	cursor->Y = 280/2;
	cfg->GetIntValue("cursorspeed", cursor->CursorSpeed);
	cursor->Display = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_cursor").c_str());
	delete(cfg);
}

void Cursor::Destroy()
{
	Graphics::FreeImage(cursor->Display);
	delete(cursor);
}

void Cursor::Handle(SceCtrlData* pad)
{
	Controller::PreviousAnalogX = cursor->X;
	Controller::PreviousAnalogY = cursor->Y;
	
	bool change = false;
	// Update position
	if (pad->Lx > 145){ cursor->X += (pad->Lx - 145) / cursor->CursorSpeed; change = true;}
	if (pad->Lx < 93) { cursor->X -= (93 - pad->Lx) / cursor->CursorSpeed; change = true;}
	if (pad->Ly > 160){ cursor->Y += (pad->Ly - 160) / cursor->CursorSpeed; change = true;}
	if (pad->Ly < 100){ cursor->Y -= (100 - pad->Ly) / cursor->CursorSpeed; change = true;}
	if (!change)
	{
		Graphics::BlitAlphaImageToScreen(0, 0, cursor->Display->imageWidth, cursor->Display->imageHeight, cursor->Display, cursor->X, cursor->Y);
		return;
	}	
	// Check if cursor is still on the screen
	if (cursor->X < 0) cursor->X=0;
	if (cursor->X > 480) cursor->X = 480;
	if (cursor->Y < 0) cursor->Y=0;
	if (cursor->Y > 272) cursor->Y = 272;
	
	Graphics::BlitAlphaImageToScreen(0, 0, cursor->Display->imageWidth, cursor->Display->imageHeight, cursor->Display, cursor->X, cursor->Y);

	Controller::AnalogX = cursor->X;
	Controller::AnalogY = cursor->Y;
}
