#include <string>
#include <malloc.h>
#include <pspkernel.h>
#include <psprtc.h>
#include <stdio.h>

#include "ASIncludes.h"


ASTaskbar::ASTaskbar()
{
	_posY = 273;
	_offsetx = 0;
	_offsety = 0;
	_startup = 1;
	
	_graphics = ASGraphics::Instance();
	_taskbar = _graphics->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_taskbar").c_str());
	_usb = _graphics->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_taskbarusb").c_str());
	_usbDis = _graphics->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_taskbarusbdis").c_str());
	_wifi = _graphics->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_taskbarwifi").c_str());
	_wifiDis = _graphics->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_taskbarwifidis").c_str());
	_screen = _graphics->LoadPNG(ASCore::MainConfigTheme->GetPathValue("c_taskbarscreen").c_str());
	
	ASCore::MainConfigTheme->GetIntValue("taskbaroffsetx", _offsetx);
	ASCore::MainConfigTheme->GetIntValue("taskbaroffsety", _offsety);
}

ASTaskbar::~ASTaskbar()
{
	delete (_taskbar);
	delete (_usb);
	delete (_usbDis);
	delete (_wifi);
	delete (_wifiDis);
	delete (_screen);
}

int ASTaskbar::GetHeight()
{
	return _taskbar->Height;
}

void ASTaskbar::Render()
{
#warning WindowManager not available yet
	//if (!WindowManager::FullScreenEnabled)
	//{
		if (_startup && _posY > 272 - (_taskbar->Height))
			_posY--;
		else
			_startup = 0;
			
		_graphics->BlitAlphaImageToScreen(0, 0, _taskbar->Width, _taskbar->Height, _taskbar, 0, _posY);
		
		if (!_startup)
		{
			//ASImage* lcd = LCD::GetLCDScreen();
			//if (lcd != NULL)
			//	_graphics->BlitAlphaImageToScreen(0, 0, lcd->imageWidth, lcd->imageHeight, lcd, 150, posY+5);
		}
		
		if (ASCore::USB->Enabled())
			_graphics->BlitAlphaImageToScreen(0, 0, _usb->Width, _usb->Height, _usb, _offsetx, _posY+_offsety);
		else
			_graphics->BlitAlphaImageToScreen(0, 0, _usbDis->Width, _usbDis->Height, _usbDis, _offsetx, _posY+_offsety);
		if (ASCore::WiFi->Connected())
			_graphics->BlitAlphaImageToScreen(0, 0, _wifi->Width, _wifi->Height, _wifi, _offsetx+25, _posY+_offsety);
		else
			_graphics->BlitAlphaImageToScreen(0, 0, _wifiDis->Width, _wifiDis->Height, _wifiDis, _offsetx+25, _posY+_offsety);
		
		_graphics->BlitAlphaImageToScreen(0, 0, _screen->Width, _screen->Height, _screen, _offsetx+50, _posY+_offsety);
		
	//}
}

void ASTaskbar::Handle()
{
	ASController* controller = ASController::Instance();
	
	if (controller->LastInput == PSP_CTRL_CROSS && ASInterface::IsWithinImage(controller->AnalogX, controller->AnalogY, 0, _posY, _taskbar) && !controller->IsLastInputPressed)
	{
		if (ASInterface::IsWithinImage(controller->AnalogX, controller->AnalogY, _offsetx, _posY+_offsety, _usb))
			ASCore::USB->ToggleUSB();
		if (ASInterface::IsWithinImage(controller->AnalogX, controller->AnalogY, _offsetx+25, _posY+_offsety, _wifi))
		{
			if (ASCore::WiFi->Connected())
				ASCore::WiFi->Disconnect();
			else
				ASCore::WiFi->Connect();
		}
	}
}
