#include <psptypes.h>
#include <malloc.h>
#include <pspctrl.h>
#include <string.h>
#include <stdio.h>
#include <psprtc.h>
#include <pspiofilemgr.h>
#include <pspkernel.h>

#include "artillery.h"

#define CONFIGPATH "ms0:/MbShell/config/main.cfg"

static int LastTime;
static int TimerEnabled;
static int HelpEnabled;
static int Fullscreen;
static int enabled = 0;
static int init = 0;
static int sizex = 0;
static int sizey = 0;
static pspTime* timed;
static Window* wnd;

Image* SlideShow::FullscreenBuffer;
FileList* SlideShow::_files;
Config* SlideShow::_cfg;
LCDScreen* SlideShow::_lcd;

void SlideShow::GetImageFiles(string path)
{
	SlideShow::_files = new FileList();
	SlideShow::_files->ReadFilesToFileList(path, "jpg");
	SlideShow::_files->ReadFilesToFileList(path, "png");
	SlideShow::_files->ReadFilesToFileList(path, "JPG");
	SlideShow::_files->ReadFilesToFileList(path, "PNG");
}

void SlideShow::LoadNextPicture(AFile* file)
{
	Image* tmp = NULL;
	if (!file)
	{
		SlideShow::_files->Reset();
		file = _files->GetNextFile();
		if (!file)
			return;
	}
	
	if (file->Filename.find("png") != string::npos)
		tmp = Graphics::LoadPNG(file->FullPath.c_str());
	else if (file->Filename.find("jpg") != string::npos)
		tmp = Graphics::LoadJPG(file->FullPath.c_str());
	else if (file->Filename.find("PNG") != string::npos)
		tmp = Graphics::LoadPNG(file->FullPath.c_str());
	else if (file->Filename.find("JPG") != string::npos)
		tmp = Graphics::LoadJPG(file->FullPath.c_str());
	else
		return;
	
	if (!Fullscreen)
		Graphics::ResizeImage(sizex,sizey, tmp, wnd->Screen);
	else
		Graphics::ResizeImage(SCREEN_WIDTH, SCREEN_HEIGHT, tmp, FullscreenBuffer);
	
	if (!Fullscreen && HelpEnabled)
	{
		Font::PrintImage("Cross: Next image", 2, 1, wnd->Screen, 0x88FFFFFF, 3, 1);
		Font::PrintImage("Square: Fullscreen", 2, 7, wnd->Screen, 0x88FFFFFF, 3, 1);
		Font::PrintImage("Triangle: Show or hide Help", 2, 13, wnd->Screen, 0x88FFFFFF, 3, 1);
		Font::PrintImage("Circle: Disable or enable timer", 2, 19, wnd->Screen, 0x88FFFFFF, 3, 1);
	}
	Graphics::FreeImage(tmp);
	
	_lcd->SetDefaultText(file->Filename);
}

void SlideShow::HandleDestroy()
{
	enabled = 0;
	init = 0;
	free(_cfg);
	LCD::RemoveScreen(_lcd);
	free(_lcd);
}

void SlideShow::HandleController(int mouseX, int mouseY, int btn)
{
	if (enabled && init && SlideShow::_files->FileCount() > -1)
	{
		if (btn == PSP_CTRL_CROSS)
		{
			LoadNextPicture(_files->GetNextFile());
			_lcd->PrintMessage("Next image");
		}
		if (btn == PSP_CTRL_SQUARE)
		{
			Fullscreen = !Fullscreen;
			wnd->Fullscreen = Fullscreen;
			if (Fullscreen)
			{
				FullscreenBuffer = Graphics::CreateImage(SCREEN_WIDTH, SCREEN_HEIGHT);
				Graphics::FreeImage(wnd->Screen);
				_lcd->PrintMessage("Fullscreen");
			}
			else
			{
				Graphics::FreeImage(FullscreenBuffer);
				wnd->Screen = Graphics::CreateImage(sizex, sizey);
				_lcd->PrintMessage("Windowed");
			}
			_files->GetPreviousFile();
			LoadNextPicture(_files->GetCurrentFile());
		}
		if (btn == PSP_CTRL_CIRCLE && Controller::PreviousInput != PSP_CTRL_CIRCLE)
		{
			TimerEnabled = !TimerEnabled;
			if (TimerEnabled)
				_lcd->PrintMessage("Timer enabled");
			else
				_lcd->PrintMessage("Timer disabled");
		}
		if (btn == PSP_CTRL_TRIANGLE && Controller::PreviousInput != PSP_CTRL_TRIANGLE)
		{
			HelpEnabled = !HelpEnabled;
			if (HelpEnabled)
				_lcd->PrintMessage("Help enabled");
			else
				_lcd->PrintMessage("Help disabled");
		}
	}
}

void SlideShow::Init()
{
	// Init config
	_cfg = new Config(CONFIGPATH);
	_cfg->GetIntValue("windowwidth", sizex);
	_cfg->GetIntValue("windowheight", sizey);
	
	// create window
	wnd = WindowManager::CreateWindow(sizex, sizey, HandleController, Render, HandleDestroy, RenderFullScreen, "slideshow", 0);
	GetImageFiles(_cfg->GetValue("imagepath"));
	
	// Create LCD screen
	_lcd = new LCDScreen();
	_lcd->PrintMessage("Slideshow");
	LCD::AddScreen(_lcd);
	
	// Init time
	timed = (pspTime*) malloc(sizeof(pspTime));
	LastTime = -1;
	enabled = 1;
	init = 1;
	Fullscreen = 0;
	HelpEnabled = 1;
	TimerEnabled = 1;
	
	if (SlideShow::_files->FileCount() > 0)
		LoadNextPicture(_files->GetCurrentFile());
}

void SlideShow::RenderFullScreen()
{
	Graphics::BlitImageToScreen(0,0,FullscreenBuffer->imageWidth, FullscreenBuffer->imageHeight, FullscreenBuffer,0,0);
	if (HelpEnabled)
	{
		Font::PrintScreen("Cross: Next image", 2, 1, 0x88FFFFFF, 3, 1);
		Font::PrintScreen("Square: Fullscreen", 2, 7, 0x88FFFFFF, 3, 1);
		Font::PrintScreen("Triangle: Show or hide Help", 2, 13, 0x88FFFFFF, 3, 1);
		Font::PrintScreen("Circle: Disable or enable timer", 2, 19, 0x88FFFFFF, 3, 1);
	}	
}

void SlideShow::Render()
{
	if (enabled)
	{
		if (SlideShow::_files->FileCount() > 0)
		{
		sceRtcGetCurrentClockLocalTime(timed);
		if ((LastTime < 50 && LastTime + 10 < timed->seconds) && TimerEnabled)
		{
			LoadNextPicture(_files->GetNextFile());
			LastTime = timed->seconds;
			return;
		}
		else if (LastTime > 49 && timed->seconds > (LastTime - 50) && timed->seconds < 50 && TimerEnabled)
		{
			LoadNextPicture(_files->GetNextFile());
			LastTime = timed->seconds;
			return;
		}
		}
		else
		{
			Font::PrintImage("No suitable images found.", sizex / 2 - 100, (sizey / 2) - 15, wnd->Screen, 0xFFFFFFFF, 3);
			Font::PrintImage("Supported: JPG, PNG", sizex / 2 - 75, (sizey / 2) - 5 , wnd->Screen, 0xFFFFFFFF, 3);
		}
	}
}
