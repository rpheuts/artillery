#include <pspctrl.h>
#include <pspsdk.h>
#include "ASIncludes.h"

ASDesktop::ASDesktop()
{
	_desktopData = ASGraphics::Instance()->Load("ms0:/Artillery/wallpapers/platinum.png"); /*new ASImage(480,272);*/
	_wallpaper = new ASImage(480,272);
	
	/*_files = new ASFileList();
	string wallpaperpath = ASCore::MainConfig->GetValue("wallpaperpath");
	_files->ReadFilesToFileList(wallpaperpath, "png");
	_files->ReadFilesToFileList(wallpaperpath, "PNG");
	_files->ReadFilesToFileList(wallpaperpath, "jpg");
	_files->ReadFilesToFileList(wallpaperpath, "JPG");
	if(_files->FileCount() == 0)
	{
		ASFile* file = new ASFile(ASCore::MainConfigTheme->GetValue("c_failsafe_wallpaper"), ASCore::MainConfigTheme->ImagesCorePath);
		LoadWallpaper(file);
		CreateDesktop();
	}
	else if (ASCore::MainConfigTheme->GetValue("prefered_wallpaper") != "none")
	{
		ASFile* file = new ASFile(ASCore::MainConfigTheme->GetValue("preferedwallpaper"), ASCore::MainConfigTheme->GetValue("preferedwallpaperpath"));
		LoadWallpaper(file);
		CreateDesktop();
	}
	else
		SwitchWallpaper();*/
}

ASDesktop::~ASDesktop()
{
	_files->Clear();
	delete (_desktopData);
	delete (_wallpaper);
}
		
void ASDesktop::LoadWallpaper(ASFile* file)
{
	ASImage* tmp = ASGraphics::Instance()->Load(file->FullPath);
	ASGraphics::Instance()->ResizeImage(SCREEN_WIDTH, SCREEN_HEIGHT, tmp, _wallpaper);
	delete (tmp);
}

void ASDesktop::Render()
{
	ASGraphics::Instance()->BlitImageToScreen(0, 0, 480, 272, _desktopData, 0, 0);
}

void ASDesktop::CreateDesktop()
{
	ASGraphics::Instance()->BlitImageToImage(0, 0, 480, 272, _wallpaper, 0, 0,_desktopData);
}

void ASDesktop::SwitchWallpaper()
{
	if(_files->FileCount() > 0)
	{
		ASFile* file = _files->GetCurrentFile();
		if (!file)
		{
			_files->Reset();
			file = _files->GetCurrentFile();
			if (!file)
				return;
		}
		LoadWallpaper(file);
		CreateDesktop();
		/*if (_init)
		{
			if (file->Filename.length() > 15)
				LCD::PrintMessage("Desktop: " + file->Filename.substr(0,15));
			else
				LCD::PrintMessage("Desktop: " + file->Filename);
		}*/
		_files->GetNextFile();
	}
}

void ASDesktop::Handle()
{
	
}
