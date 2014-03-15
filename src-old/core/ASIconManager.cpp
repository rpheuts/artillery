#include <malloc.h>
#include <pspctrl.h>
#include "ASIncludes.h"

#include <pspdebug.h>

ASIconManager::ASIconManager()
{
	// Load icons from config file and display the ones needed on desktop
	list<string> hblist;
	ASConfigIcons* cfgi = new ASConfigIcons(ASCore::MainConfigTheme->IconConfig);
	cfgi->FillIconList(&hblist);
	_gridSize = 0;
	ASCore::MainConfig->GetIntValue("gridsize", _gridSize);
	ASGraphics* _graphics = ASGraphics::Instance();
	_listStart = 0;
	
	//Lets run through the list and add icons to the desktop
	while (hblist.size() > 0)
	{
		if (cfgi->CheckDesktopValue(hblist.front()))
		{
			string path = cfgi->GetPathValue(hblist.front());
			if (path.substr(0,1) == "_")
			{
				
				
				if (path == "_usb")
					RegisterIcon(new ASIcon(cfgi->GetPosXValue(hblist.front()), cfgi->GetPosYValue(hblist.front()), _graphics->LoadPNG(cfgi->GetIconValue(hblist.front()).c_str()), _graphics->LoadPNG(cfgi->GetIconSelValue(hblist.front()).c_str()), 0 /*ASCore::USB->ToggleUSB*/, hblist.front()));
				else if (path == "_umd")
					RegisterIcon(new ASIcon(cfgi->GetPosXValue(hblist.front()), cfgi->GetPosYValue(hblist.front()), _graphics->LoadPNG(cfgi->GetIconValue(hblist.front()).c_str()), _graphics->LoadPNG(cfgi->GetIconSelValue(hblist.front()).c_str()), 0 /*ModuleManager::LoadStartUMD*/, hblist.front()));
				/*else if (path == "_photo")
					RegisterIcon(new ASIcon(cfgi->GetPosXValue(hblist.front()), cfgi->GetPosYValue(hblist.front()), _graphics->LoadPNG(cfgi->GetIconValue(hblist.front()).c_str()), _graphics->LoadPNG(cfgi->GetIconSelValue(hblist.front()).c_str()), SlideShow::Init, "Slideshow viewer"));
				else if (path == "_music")
					RegisterIcon(new ASIcon(cfgi->GetPosXValue(hblist.front()), cfgi->GetPosYValue(hblist.front()), _graphics->LoadPNG(cfgi->GetIconValue(hblist.front()).c_str()), _graphics->LoadPNG(cfgi->GetIconSelValue(hblist.front()).c_str()), MP3Player::Init, "MP3 Player"));
				else if (path == "_read")
					RegisterIcon(new ASIcon(cfgi->GetPosXValue(hblist.front()), cfgi->GetPosYValue(hblist.front()), _graphics->LoadPNG(cfgi->GetIconValue(hblist.front()).c_str()), _graphics->LoadPNG(cfgi->GetIconSelValue(hblist.front()).c_str()), Reader::Init, "Reader"));
				else if (path == "_hb")
					RegisterIcon(new ASIcon(cfgi->GetPosXValue(hblist.front()), cfgi->GetPosYValue(hblist.front()), _graphics->LoadPNG(cfgi->GetIconValue(hblist.front()).c_str()), _graphics->LoadPNG(cfgi->GetIconSelValue(hblist.front()).c_str()), HomeBrewManager::Init, "Homebrew"));*/
			}
			else
				RegisterIcon(new ASIcon(cfgi->GetPosXValue(hblist.front()), cfgi->GetPosYValue(hblist.front()), _graphics->LoadPNG(cfgi->GetIconValue(hblist.front()).c_str()), _graphics->LoadPNG(cfgi->GetIconSelValue(hblist.front()).c_str()), path, hblist.front()));
		}
		hblist.pop_front();	
	}
	delete (cfgi);
	delete (_graphics);
}

ASIconManager::~ASIconManager()
{

	ASIcon* t = _listStart;
	ASIcon* rem = NULL;
	while (t)
	{
		delete (t->Display);
		delete (t->DisplaySel);
		rem = t;
		t = t->Next;
		delete(rem);
	}
	_listStart = NULL;
}

void ASIconManager::RegisterIcon(ASIcon* icon)
{
	if (!_listStart)
	{
		_listStart = icon;
		_listStart->Next = 0;
		_isDraging = 0;
	}
	else
	{
		// Add to the beginning of the list, most easy :]
		icon->Next =  _listStart;
		_listStart = icon;
	}
}

ASIcon* ASIconManager::CreateIcon(int x, int y, ASImage* image, ASImage* imageSel, void (*ptf)(void), string name)
{
	ASIcon* i = new ASIcon(x, y, image, imageSel, ptf, name);
	RegisterIcon(i);
	return i;
}

void ASIconManager::SetIconPosition(ASIcon* icon, int x, int y)
{
	icon->X = x;
	icon->Y = y;
}

void ASIconManager::Render()
{
	ASIcon* t = _listStart;
	while (t)
	{
		if (!t->Dragged || !ASController::Instance()->IsLastInputPressed)
			SetIconPosition(t, (((t->X/_gridSize)*_gridSize)+5) + ((_gridSize / 2) - (t->Display->Width / 2)), (((t->Y/_gridSize)*_gridSize)+3) + ((_gridSize / 2) - (t->Display->Height / 2)));
		if (t->Selected)
			ASGraphics::Instance()->BlitAlphaImageToScreen(0, 0, t->DisplaySel->Width, t->DisplaySel->Height, t->DisplaySel, t->X, t->Y);
		else
			ASGraphics::Instance()->BlitAlphaImageToScreen(0, 0, t->Display->Width, t->Display->Height, t->Display, t->X, t->Y);
		t = t->Next;
	}
}

void ASIconManager::HandleIconDrag()
{
	ASController* controller = ASController::Instance();
	if (controller->LastInput == PSP_CTRL_CROSS && _listStart)
	{
	ASIcon* t = _listStart;
		while (t)
		{
			if (ASInterface::IsWithinImage(controller->AnalogX, controller->AnalogY, t->X, t->Y, t->Display))
				if ((!controller->IsLastInputPressed && !t->Dragged && !_isDraging) || t->Dragged)
				{
					SetIconPosition(t, t->X + (controller->AnalogX - controller->PreviousAnalogX), t->Y + (controller->AnalogY - controller->PreviousAnalogY));
					t->Dragged = 1;
					_isDraging = 1;
					controller->Handled = 1;
					// continue to set other dragged icons to 0
					t = t->Next;
					continue;
				}
			t->Dragged = 0;
			t = t->Next;
		}
	}
}

void ASIconManager::HandleIconClick()
{
	ASController* controller = ASController::Instance();
	if (controller->LastInput == PSP_CTRL_CROSS && !controller->IsLastInputPressed && _listStart)
	{
		ASIcon *t = _listStart;
		while (t)
		{
			if (ASInterface::IsWithinImage(controller->AnalogX, controller->AnalogY, t->X, t->Y, t->Display))
			{
				controller->Handled = 1;
				if (t->Selected)
					if (t->InternalFunction == 0)
						t->Run();
					else
						t->RunInternal();
				else
				{
					t->Selected = 1;
					//LCD::PrintMessage(t->Name);
					t = t->Next;
					continue;
				}
			}
			t->Selected = 0;
			t = t->Next;
		}
	}
}

void ASIconManager::Handle()
{
	if (!ASController::Instance()->IsLastInputPressed)
		_isDraging = 0;
	if (!ASController::Instance()->Handled)
	{
		HandleIconClick();
		HandleIconDrag();
	}
}

ASIcon::ASIcon(int x, int y, ASImage* image, ASImage* imageSel, void (*ptf)(void), string name)
{
	X = x;
	Y = y;
	Width = image->Width;
	Height = image->Height;
	Selected = 0;
	Dragged = 0;
	Run = ptf;
	DisplaySel = imageSel;
	Display = image;
	InternalFunction = 0;
	Name = name;
}

ASIcon::ASIcon(int x, int y, ASImage* image, ASImage* imageSel, string path, string name)
{
	X = x;
	Y = y;
	Width = image->Width;
	Height = image->Height;
	Selected = 0;
	Dragged = 0;
	Run = NULL;
	DisplaySel = imageSel;
	Display = image;
	InternalFunction = 1;
	Path = path;
	Name = name;
}

void ASIcon::RunInternal()
{
	//ModuleManager::LoadStartELF(Path.c_str());
}
