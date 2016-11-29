#include "artillery.h"

#define THEME_PATH "ms0:/MbShell/themes/"
#define THEME_FILE "/theme.cfg"
ConfigTheme::ConfigTheme(string themeName) : Config("")
{
	Config* cfg = new Config("ms0:/MbShell/config/main.cfg");
	configFile = THEME_PATH + cfg->GetValue("theme") + THEME_FILE;
	ImagesCorePath = THEME_PATH + cfg->GetValue("theme") + "/core/";
	ImagesMP3Path = THEME_PATH + cfg->GetValue("theme") + "/mp3player/";
	ImagesIconPath = THEME_PATH + cfg->GetValue("theme") + "/icons/";
	ImagesBootPath = THEME_PATH + cfg->GetValue("theme") + "/boot/";
	IconConfig = THEME_PATH + cfg->GetValue("theme") + "/icons.cfg";
	ThemeDir = THEME_PATH;
}

string ConfigTheme::GetPathValue(string key)
{
	if (key[0] == 'c')
		return ImagesCorePath + Config::GetValue(key);
	else if (key[0] == 'm')
		return ImagesMP3Path + Config::GetValue(key);
	else if (key[0] == 'i')
		return ImagesIconPath + Config::GetValue(key);
	else if (key[0] == 'b')
		return ImagesBootPath + Config::GetValue(key);
	else return NULL;
}

string ConfigTheme::GetValue(string key)
{
	return Config::GetValue(key);
}

void ConfigTheme::SetValue(string key, string value)
{
	Config::SetValue(key, value);
}
