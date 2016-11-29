#include "artillery.h"
#include <pspctrl.h>

static Image* wallpaper;
static Image* desktopData;
FileList* Desktop::_files;
int Desktop::_init = 0;

void Desktop::LoadWallpaper(AFile* file)
{
	Image* tmp;
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
	
	Graphics::ResizeImage(SCREEN_WIDTH, SCREEN_HEIGHT, tmp, wallpaper);
	Graphics::FreeImage(tmp);
}

void Desktop::Render()
{
	Graphics::BlitImageToScreen(0, 0, 480, 272, desktopData, 0, 0);
}

void Desktop::CreateDesktop()
{
	Graphics::BlitImageToImage(0, 0, 480, 272, wallpaper, 0, 0,desktopData);
}

void Desktop::SwitchWallpaper()
{
	if(_files->FileCount() > 0)
	{
		AFile* file = _files->GetCurrentFile();
		if (!file)
		{
			_files->Reset();
			file = _files->GetCurrentFile();
			if (!file)
				return;
		}
		LoadWallpaper(file);
		CreateDesktop();
		if (_init)
		{
			if (file->Filename.length() > 15)
				LCD::PrintMessage("Desktop: " + file->Filename.substr(0,15));
			else
				LCD::PrintMessage("Desktop: " + file->Filename);
		}
		_files->GetNextFile();
	}
}

void Desktop::Handle()
{
	if (Controller::LastInput == PSP_CTRL_RTRIGGER && !Controller::IsLastInputPressed)
	{
		if (!HomeBrewManager::IsInit)
		{
			HomeBrewManager::Init();
			return;	
		}
		if(HomeBrewManager::Container->Visible)
			HomeBrewManager::Container->Hide();
		else
			HomeBrewManager::Container->Show();
	}
	if (Controller::LastInput == PSP_CTRL_CROSS && !Controller::IsLastInputPressed)
	{
		if (PopupTheme::Visible())
			PopupTheme::Hide();
	}
	Controller::Handled = 1;
}

void Desktop::Init()
{
	desktopData = Graphics::CreateImage(480,272);
	wallpaper = Graphics::CreateImage(480,272);
	_files = new FileList();
	Config* cfg = new Config("ms0:/MBShell/config/main.cfg");
	_files->ReadFilesToFileList(cfg->GetValue("wallpaperpath"), "png");
	_files->ReadFilesToFileList(cfg->GetValue("wallpaperpath"), "PNG");
	_files->ReadFilesToFileList(cfg->GetValue("wallpaperpath"), "jpg");
	_files->ReadFilesToFileList(cfg->GetValue("wallpaperpath"), "JPG");
	if(_files->FileCount() == 0)
	{
		AFile* file = new AFile(Core::CfgTheme->GetValue("c_failsafewallpaper").c_str(), Core::CfgTheme->ImagesCorePath.c_str());
		LoadWallpaper(file);
		CreateDesktop();
	}
	else if (Core::CfgTheme->GetValue("preferedwallpaper") != "none")
	{
		AFile* file = new AFile(Core::CfgTheme->GetValue("preferedwallpaper").c_str(), Core::CfgTheme->GetValue("preferedwallpaperpath").c_str());
		LoadWallpaper(file);
		CreateDesktop();
	}
	else
		SwitchWallpaper();
	_init = 1;
	free (cfg);
}

void Desktop::Destroy()
{
	//_files->Clear();
	Graphics::FreeImage(desktopData);
	Graphics::FreeImage(wallpaper);
	_init = 0;
}
