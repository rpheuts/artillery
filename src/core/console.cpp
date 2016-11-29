#include "artillery.h"

static Window* wndHnd;
static Image* img_back_panel;

#ifdef DBG
static int cursorX = 3, cursorY = 3;
#endif

void Console::HandleController(int analogX, int analogY, int btn)
{
	// do nothing :P
}

void Console::Render()
{
	// do nothing :P
}

void Console::HandleDestroy()
{
	Graphics::FreeImage(img_back_panel);
}

void Console::Init()
{
	img_back_panel = Graphics::CreateImage(270,170);
	Graphics::ClearImage(0xAA000000, img_back_panel);
	
	// register window
	wndHnd = WindowManager::CreateWindow(img_back_panel->imageWidth, img_back_panel->imageHeight, HandleController, Render, HandleDestroy, "console", 1);
	wndHnd->Screen = Graphics::CreateImage(img_back_panel->imageWidth, img_back_panel->imageHeight);
	Graphics::BlitImageToImage(0,0, img_back_panel->imageWidth, img_back_panel->imageHeight, img_back_panel, 0, 0, wndHnd->Screen);
}

void Console::Print(string text)
{
	Print(text.c_str());
}

void Console::Print(char * text)
{
	#ifdef DBG
	Font::PrintImage(text, cursorX, cursorY, wndHnd->Screen, 0xFFFFFFFF, 3);
	cursorY += 6;
	cursorY += 6;
	if (cursorY > img_back_panel->imageHeight)
	{
		Graphics::BlitImageToImage(0,0, img_back_panel->imageWidth, img_back_panel->imageHeight, img_back_panel, 0, 0, wndHnd->Screen);
		cursorY = 3;
	}
	#endif
}

void Console::Print(const char * text)
{
	#ifdef DBG
	Font::PrintImage(text, cursorX, cursorY, wndHnd->Screen, 0xFFFFFFFF, 3);
	cursorY += 6;
	if (cursorY > img_back_panel->imageHeight)
	{
		Graphics::BlitImageToImage(0,0, img_back_panel->imageWidth, img_back_panel->imageHeight, img_back_panel, 0, 0, wndHnd->Screen);
		cursorY = 3;
	}
	#endif
}
