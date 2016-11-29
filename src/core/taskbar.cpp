#include <string>
#include <malloc.h>
#include <pspkernel.h>
#include <psprtc.h>
#include <stdio.h>

#include "artillery.h"

Image* taskbar;
Image* usb;
Image* usb_dis;
Image* wifi;
Image* wifi_dis;
Image* screen;

int startup = 1;
int posY = 273;
int offsetx = 0;
int offsety = 0;

void Taskbar::Init()
{
	taskbar = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_taskbar").c_str());
	usb = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_taskbarusb").c_str());
	usb_dis = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_taskbarusbdis").c_str());
	wifi = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_taskbarwifi").c_str());
	wifi_dis = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_taskbarwifidis").c_str());
	screen = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_taskbarscreen").c_str());
	
	Core::CfgTheme->GetIntValue("taskbaroffsetx", offsetx);
	Core::CfgTheme->GetIntValue("taskbaroffsety", offsety);
}

int Taskbar::GetHeight()
{
	return taskbar->imageHeight;
}

void Taskbar::Destroy()
{
	Graphics::FreeImage(taskbar);
	Graphics::FreeImage(usb);
	Graphics::FreeImage(usb_dis);
	Graphics::FreeImage(wifi);
	Graphics::FreeImage(wifi_dis);
	Graphics::FreeImage(screen);
}

void Taskbar::Render()
{
	if (!WindowManager::FullScreenEnabled)
	{
		if (startup && posY > 272 - (taskbar->imageHeight))
			posY--;
		else
			startup = 0;
			
		Graphics::BlitAlphaImageToScreen(0, 0, taskbar->imageWidth, taskbar->imageHeight, taskbar, 0, posY);
		
		if (!startup)
		{
			Image* lcd = LCD::GetLCDScreen();
			if (lcd != NULL)
				Graphics::BlitAlphaImageToScreen(0, 0, lcd->imageWidth, lcd->imageHeight, lcd, 150, posY+5);
		}
		
		if (USBManager::IsEnabled())
			Graphics::BlitAlphaImageToScreen(0, 0, usb->imageWidth, usb->imageHeight, usb, offsetx, posY+offsety);
		else
			Graphics::BlitAlphaImageToScreen(0, 0, usb_dis->imageWidth, usb_dis->imageHeight, usb_dis, offsetx, posY+offsety);
		if (WiFiManager::IsConnected())
			Graphics::BlitAlphaImageToScreen(0, 0, usb->imageWidth, usb->imageHeight, wifi, offsetx+25, posY+offsety);
		else
			Graphics::BlitAlphaImageToScreen(0, 0, usb_dis->imageWidth, usb_dis->imageHeight, wifi_dis, offsetx+25, posY+offsety);
		
		Graphics::BlitAlphaImageToScreen(0, 0, screen->imageWidth, screen->imageHeight, screen, offsetx+50, posY+offsety);
		
	}
}

void Taskbar::Handle()
{
	if (Controller::LastInput == PSP_CTRL_CROSS && Interface::IsWithinImage(Controller::AnalogX, Controller::AnalogY, 0, posY, taskbar) && !Controller::IsLastInputPressed)
	{
		if (Interface::IsWithinImage(Controller::AnalogX, Controller::AnalogY, offsetx, posY+offsety, usb))
			USBManager::ToggleUSB();
		if (Interface::IsWithinImage(Controller::AnalogX, Controller::AnalogY, offsetx+25, posY+offsety, wifi))
		{
			if (WiFiManager::IsConnected())
				WiFiManager::Terminate();
			else
				WiFiManager::Connect();
		}
		if (Interface::IsWithinImage(Controller::AnalogX, Controller::AnalogY, offsetx+50, posY+offsety, screen))
		{
			if (!PopupTheme::Visible())
				PopupTheme::TogglePopup();
			else
				PopupTheme::Hide();
		}
	}
}
