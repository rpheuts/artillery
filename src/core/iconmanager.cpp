#include <malloc.h>
#include <pspctrl.h>
#include "artillery.h"

#include <pspdebug.h>
#define printf	pspDebugScreenPrintf

static Icon* ListStart;
static int isDraging;
static int GridSize;
static int icInit;

void IconManager::Init()
{
	// Load icons from config file and display the ones needed on desktop
	list<string> hblist;
	ConfigHomebrew* cfgh = new ConfigHomebrew(Core::CfgTheme->IconConfig);
	cfgh->FillIconList(&hblist);
	GridSize = 0;
	Core::Cfg->GetIntValue("gridsize", GridSize);
	
	//Lets run through the list and add icons to the desktop
	while (hblist.size() > 0)
	{
		if (cfgh->CheckDesktopValue(hblist.front()))
		{
			string path = cfgh->GetPathValue(hblist.front());
			if (path.substr(0,1) == "_")
			{
				
				if (path == "_usb")
					RegisterIcon(new Icon(cfgh->GetPosXValue(hblist.front()), cfgh->GetPosYValue(hblist.front()), Graphics::LoadPNG(cfgh->GetIconValue(hblist.front()).c_str()), Graphics::LoadPNG(cfgh->GetIconSelValue(hblist.front()).c_str()), USBManager::ToggleUSB, hblist.front()));
				else if (path == "_umd")
					RegisterIcon(new Icon(cfgh->GetPosXValue(hblist.front()), cfgh->GetPosYValue(hblist.front()), Graphics::LoadPNG(cfgh->GetIconValue(hblist.front()).c_str()), Graphics::LoadPNG(cfgh->GetIconSelValue(hblist.front()).c_str()), ModuleManager::LoadStartUMD, hblist.front()));
				else if (path == "_photo")
					RegisterIcon(new Icon(cfgh->GetPosXValue(hblist.front()), cfgh->GetPosYValue(hblist.front()), Graphics::LoadPNG(cfgh->GetIconValue(hblist.front()).c_str()), Graphics::LoadPNG(cfgh->GetIconSelValue(hblist.front()).c_str()), SlideShow::Init, "Slideshow viewer"));
				else if (path == "_music")
					RegisterIcon(new Icon(cfgh->GetPosXValue(hblist.front()), cfgh->GetPosYValue(hblist.front()), Graphics::LoadPNG(cfgh->GetIconValue(hblist.front()).c_str()), Graphics::LoadPNG(cfgh->GetIconSelValue(hblist.front()).c_str()), MP3Player::Init, "MP3 Player"));
				else if (path == "_read")
					RegisterIcon(new Icon(cfgh->GetPosXValue(hblist.front()), cfgh->GetPosYValue(hblist.front()), Graphics::LoadPNG(cfgh->GetIconValue(hblist.front()).c_str()), Graphics::LoadPNG(cfgh->GetIconSelValue(hblist.front()).c_str()), Reader::Init, "Reader"));
				else if (path == "_hb")
					RegisterIcon(new Icon(cfgh->GetPosXValue(hblist.front()), cfgh->GetPosYValue(hblist.front()), Graphics::LoadPNG(cfgh->GetIconValue(hblist.front()).c_str()), Graphics::LoadPNG(cfgh->GetIconSelValue(hblist.front()).c_str()), HomeBrewManager::Init, "Homebrew"));
			}
			else
				RegisterIcon(new Icon(cfgh->GetPosXValue(hblist.front()), cfgh->GetPosYValue(hblist.front()), Graphics::LoadPNG(cfgh->GetIconValue(hblist.front()).c_str()), Graphics::LoadPNG(cfgh->GetIconSelValue(hblist.front()).c_str()), path, hblist.front()));
		}
		hblist.pop_front();	
	}
	delete(cfgh);
	icInit = 1;
}

void IconManager::Destroy()
{
	if (icInit == 1)
	{
		Icon* t = ListStart;
		Icon* rem = NULL;
		while (t)
		{
			Graphics::FreeImage(t->Display);
			Graphics::FreeImage(t->DisplaySel);
			rem = t;
			t = t->Next;
			delete(rem);
		}
	}
	icInit = 0;
	ListStart = NULL;
}

void IconManager::RegisterIcon(Icon* icon)
{
	if (!ListStart)
	{
		ListStart = icon;
		ListStart->Next = 0;
		isDraging = 0;
	}
	else
	{
		// Add to the beginning of the list, most easy :]
		icon->Next =  ListStart;
		ListStart = icon;
	}
}

Icon* IconManager::CreateIcon(int x, int y, Image* image, Image* imageSel, void (*ptf)(void), string name)
{
	Icon* i = new Icon(x, y, image, imageSel, ptf, name);
	RegisterIcon(i);
	return i;
}

void IconManager::SetIconPosition(Icon* icon, int x, int y)
{
	icon->X = x;
	icon->Y = y;
}

void IconManager::Render()
{
	Icon* t = ListStart;
	while (t)
	{
		if (!t->Dragged || !Controller::IsLastInputPressed)
			SetIconPosition(t, (((t->X/GridSize)*GridSize)+5) + ((GridSize / 2) - (t->Display->imageWidth / 2)), (((t->Y/GridSize)*GridSize)+3) + ((GridSize / 2) - (t->Display->imageHeight / 2)));
		if (t->Selected)
			Graphics::BlitAlphaImageToScreen(0, 0, t->DisplaySel->imageWidth, t->DisplaySel->imageHeight, t->DisplaySel, t->X, t->Y);
		else
			Graphics::BlitAlphaImageToScreen(0, 0, t->Display->imageWidth, t->Display->imageHeight, t->Display, t->X, t->Y);
		t = t->Next;
	}
}

void IconManager::HandleIconDrag()
{
	if (Controller::LastInput == PSP_CTRL_CROSS && ListStart)
	{
	Icon* t = ListStart;
		while (t)
		{
			if (Interface::IsWithinImage(Controller::AnalogX, Controller::AnalogY, t->X, t->Y, t->Display))
				if ((!Controller::IsLastInputPressed && !t->Dragged && !isDraging) || t->Dragged)
				{
					SetIconPosition(t, t->X + (Controller::AnalogX - Controller::PreviousAnalogX), t->Y + (Controller::AnalogY - Controller::PreviousAnalogY));
					t->Dragged = 1;
					isDraging = 1;
					Controller::Handled = 1;
					// continue to set other dragged icons to 0
					t = t->Next;
					continue;
				}
			t->Dragged = 0;
			t = t->Next;
		}
	}
}

void IconManager::HandleIconClick()
{
	if (Controller::LastInput == PSP_CTRL_CROSS && !Controller::IsLastInputPressed && ListStart)
	{
		Icon *t = ListStart;
		while (t)
		{
			if (Interface::IsWithinImage(Controller::AnalogX, Controller::AnalogY, t->X, t->Y, t->Display))
			{
				Controller::Handled = 1;
				if (t->Selected)
					if (t->InternalFunction == 0)
						t->Run();
					else
						t->RunInternal();
				else
				{
					t->Selected = 1;
					LCD::PrintMessage(t->Name);
					t = t->Next;
					continue;
				}
			}
			t->Selected = 0;
			t = t->Next;
		}
	}
}

void IconManager::Handle()
{
	if (!Controller::IsLastInputPressed)
		isDraging = 0;
	if (!Controller::Handled)
	{
		HandleIconClick();
		HandleIconDrag();
	}
}

Icon::Icon(int x, int y, Image* image, Image* imageSel, void (*ptf)(void), string name)
{
	X = x;
	Y = y;
	Width = image->imageWidth;
	Height = image->imageHeight;
	Selected = 0;
	Dragged = 0;
	Run = ptf;
	DisplaySel = imageSel;
	Display = image;
	InternalFunction = 0;
	Name = name;
}

Icon::Icon(int x, int y, Image* image, Image* imageSel, string path, string name)
{
	X = x;
	Y = y;
	Width = image->imageWidth;
	Height = image->imageHeight;
	Selected = 0;
	Dragged = 0;
	Run = NULL;
	DisplaySel = imageSel;
	Display = image;
	InternalFunction = 1;
	Path = path;
	Name = name;
}

void Icon::RunInternal()
{
	ModuleManager::LoadStartELF(Path.c_str());
}
