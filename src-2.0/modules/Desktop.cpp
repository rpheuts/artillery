#include "Includes.h"

Desktop* Desktop::_instance = 0;

Desktop* Desktop::Instance()
{
	if (_instance == 0)
		_instance = new Desktop();
	return _instance;
}

void Desktop::Tick()
{
	_wallpaper->BlitToScreen(0, 0);
}
		
Desktop::Desktop() : Module (2, "DesktopModule", 1)
{
	_wallpaper = Graphics::Instance()->Load(Core::MainConfigTheme->GetValue("preferedwallpaperpath") +
			Core::MainConfigTheme->GetValue("preferedwallpaper"));
}

Desktop::~Desktop()
{
	delete (_wallpaper);
}
