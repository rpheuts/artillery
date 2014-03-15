#include "ASIncludes.h"

ASWindowApplication::ASWindowApplication(int width, int height)
{
	Screen = new ASImage(width, height);
	Width = width;
	Height = height;
	X = 0;
	Y = 0;
}

ASWindowApplication::~ASWindowApplication()
{

}

void ASWindowApplication::HandleController(int mouseX, int mouseY, int btn)
{

}

void ASWindowApplication::HandleInputEvent(string EventID, string arg)
{

}

void ASWindowApplication::Destroy()
{

}

void ASWindowApplication::Render()
{

}

void ASWindowApplication::RenderFullscreen()
{

}
