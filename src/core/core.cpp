#include <pspmoduleinfo.h> 
#include <pspaudiolib.h>
#include <pspctrl.h>
#include <psppower.h>
#include "artillery.h"

PSP_MODULE_INFO("COREPRX", 0, 1, 0);
PSP_HEAP_SIZE_KB(20480);

static SceCtrlData pad;
ConfigTheme* Core::CfgTheme;
Config* Core::Cfg;

bool Core::Init()
{
	// Open config file
	Config* cfgBoot = new Config("ms0:/MbShell/config/boot.cfg");
	Cfg = new Config("ms0:/MbShell/config/main.cfg");
	CfgTheme = new ConfigTheme(Cfg->GetValue("theme"));

	// Init all the shell components
	Graphics::Init();
	Image * screenbuffer = Graphics::LoadPNG(CfgTheme->GetPathValue("b_background").c_str());
	if (!screenbuffer || !cfgBoot->CheckValue("showbuffer"))
	{
		screenbuffer = Graphics::CreateImage(480, 272);
		Graphics::ClearImage(0x00000000, screenbuffer);
	}
	Graphics::BlitImageToScreen(0, 0, screenbuffer->imageWidth, screenbuffer->imageHeight, screenbuffer, 0, 0);
	Graphics::FlipScreen();
	
	Image * system = Graphics::LoadPNG(CfgTheme->GetPathValue("b_system").c_str());
	Image * bar = Graphics::LoadPNG(CfgTheme->GetPathValue("b_bar").c_str());
	Graphics::BlitAlphaImageToImage(0, 0, system->imageWidth, system->imageHeight, system, 75, 90, screenbuffer);
	Graphics::BlitAlphaImageToImage(0, 0, bar->imageWidth-250, bar->imageHeight, bar, 101, 163, screenbuffer);
	Graphics::BlitImageToScreen(0, 0, screenbuffer->imageWidth, screenbuffer->imageHeight, screenbuffer, 0, 0);
	Graphics::FlipScreen();
	
	Desktop::Init();
	Taskbar::Init();
	LCD::Init();
	
	Image * controller = Graphics::LoadPNG(CfgTheme->GetPathValue("b_controller").c_str());
	Graphics::BlitAlphaImageToImage(0, 0, controller->imageWidth, controller->imageHeight, controller, 170, 90, screenbuffer);
	Graphics::BlitAlphaImageToImage(0, 0, bar->imageWidth-150, bar->imageHeight, bar, 101, 163, screenbuffer);
	Graphics::BlitImageToScreen(0, 0, screenbuffer->imageWidth, screenbuffer->imageHeight, screenbuffer, 0, 0);
	Graphics::FlipScreen();
	
	Cursor::Init();
	Controller::Init();
	
	Image * wm = Graphics::LoadPNG(CfgTheme->GetPathValue("b_windowmanager").c_str());
	Graphics::BlitAlphaImageToImage(0, 0, wm->imageWidth, wm->imageHeight, wm, 255, 90, screenbuffer);
	Graphics::BlitAlphaImageToImage(0, 0, bar->imageWidth-75, bar->imageHeight, bar, 101, 163, screenbuffer);
	Graphics::BlitImageToScreen(0, 0, screenbuffer->imageWidth, screenbuffer->imageHeight, screenbuffer, 0, 0);
	Graphics::FlipScreen();
	
	WindowManager::Init();
	
	if (cfgBoot->CheckValue("iconmanager"))
	{
		IconManager::Init();
	}
	
	PluginManager::Init();
	pspAudioInit();
	
	Image * desktop = Graphics::LoadPNG(CfgTheme->GetPathValue("b_desktop").c_str());
	Graphics::BlitAlphaImageToImage(0, 0, desktop->imageWidth, desktop->imageHeight, desktop, 340, 90, screenbuffer);
	Graphics::BlitAlphaImageToImage(0, 0, bar->imageWidth, bar->imageHeight, bar, 101, 163, screenbuffer);
	Graphics::BlitImageToScreen(0, 0, screenbuffer->imageWidth, screenbuffer->imageHeight, screenbuffer, 0, 0);
	Graphics::FlipScreen();
	
	if (cfgBoot->CheckValue("homebrewloader"))
	{
		HomeBrewManager::Init();
		HomeBrewManager::Container->Hide();
	}
	
	if (cfgBoot->CheckValue("startusb"))
		USBManager::ToggleUSB(1);
		
	Graphics::FreeImage(controller);
	Graphics::FreeImage(system);
	Graphics::FreeImage(wm);
	Graphics::FreeImage(desktop);
	Graphics::FreeImage(bar);
	Graphics::FreeImage(screenbuffer);
	
	#ifdef DBG
	// Debug console
	Console::Init();
	Console::Print("Artillery Debug Console");
	#endif
	delete (cfgBoot);
	return true;
}

void Core::Reload()
{
	Config* cfgBoot = new Config("ms0:/MbShell/config/boot.cfg");
	Cfg = new Config("ms0:/MbShell/config/main.cfg");
	CfgTheme = new ConfigTheme(Cfg->GetValue("theme"));
	
	Desktop::Init();
	Taskbar::Init();
	LCD::Init();
	Cursor::Init();
	WindowManager::Init();
	if (cfgBoot->CheckValue("homebrewloader"))
	{
		//HomeBrewManager::Init();
		//HomeBrewManager::Container->Hide();
	}
	pspAudioInit();
	delete (cfgBoot);
}

void Core::Destroy()
{
	WindowManager::Destroy();
	HomeBrewManager::Destroy();
	Cursor::Destroy();
	Desktop::Destroy();
	Taskbar::Destroy();
	LCD::Destroy();
	delete(Cfg);
	delete(CfgTheme);
	pspAudioEnd();
}

/* Main core function */
void Core::Process()
{
		// Read in current control info
		sceCtrlReadBufferPositive(&pad, 1);
		
		// Render the shell components
		Desktop::Render();
		IconManager::Render();
		PluginManager::Handle();
		WindowManager::Render();
		Taskbar::Render();
		PopupTheme::Render();
		
		// Handle the controller
		Cursor::Handle(&pad);
		Controller::Handle(&pad);
		if (!Controller::Handled)
			Taskbar::Handle();
		if (!Controller::Handled)
			WindowManager::Handle();
		if (!Controller::Handled)
			IconManager::Handle();
		if (!Controller::Handled)
			Desktop::Handle();
		
		// Update screen
		Graphics::WaitVblankStart();
		Graphics::FlipScreen();
}
