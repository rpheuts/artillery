#include "Includes.h"

WindowApplication::WindowApplication(string name, int width, int height)
{
	this->Settings = new WindowSettings();
	Settings->Borders = 1;
	Settings->Enabled = 1;
	Settings->Fullscreen = 0;
	Settings->Transparent = 0;
	Settings->OnTop = 0;
	this->Width = width;
	this->Height = height;
	this->X = -1;
	this->Y = -1;
	this->Name = name;
	Screen = new Image(width, height);
}

WindowApplication::WindowApplication(string name, int width, int height, WindowSettings* settings)
{
	this->Settings = settings;
	this->Width = width;
	this->Height = height;
	this->X = -1;
	this->Y = -1;
	this->Name = name;
	Screen = new Image(width, height);
}

WindowApplication::WindowApplication(string name, int width, int height, int borders, int enabled, int fullscreen, int transparent, int onTop)
{
	this->Width = width;
	this->Height = height;
	this->X = -1;
	this->Y = -1;
	this->Settings = new WindowSettings();
	Settings->Borders = borders;
	Settings->Enabled = enabled;
	Settings->Fullscreen = fullscreen;
	Settings->Transparent = transparent;
	Settings->OnTop = onTop;
	this->Name = name;
	Screen = new Image(width, height);
}

WindowApplication::~WindowApplication()
{
	delete (Settings);
}

void WindowApplication::Focus()
{
	ArtilleryWindowManager::Instance()->Focus(this);
}
		
void WindowApplication::Destroy()
{
	ArtilleryWindowManager::Instance()->DestroyWindow(this);
}

void WindowApplication::Position(int x, int y)
{
	ArtilleryWindowManager::Instance()->WindowPosition(this, x, y);
}

Image* WindowApplication::RenderToImage()
{
	return ArtilleryWindowManager::Instance()->RenderWindowToImage(this);
}

void WindowApplication::Register()
{
	ArtilleryWindowManager::Instance()->RegisterWindow(this);
}

void WindowApplication::HandleController(int mouseX, int mouseY)
{

}

void WindowApplication::DestroyHandler()
{

}

void WindowApplication::Render()
{

}

void WindowApplication::RenderFullscreen()
{

}
