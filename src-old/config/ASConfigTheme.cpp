#include "ASIncludes.h"

ASConfigTheme::ASConfigTheme() : ASConfig("")
{
	configFile = THEME_PATH + ASCore::MainConfig->GetValue("theme") + THEME_FILE;
	ImagesCorePath = THEME_PATH + ASCore::MainConfig->GetValue("theme") + "/core/";
	ImagesBootPath = THEME_PATH + ASCore::MainConfig->GetValue("theme") + "/boot/";
	IconConfig = THEME_PATH + ASCore::MainConfig->GetValue("theme") + "/icons.cfg";
	ThemeDir = THEME_PATH;
}

string ASConfigTheme::GetPathValue(string key)
{
	if (key[0] == 'c')
		return ImagesCorePath + ASConfig::GetValue(key);
	else if (key[0] == 'b')
		return ImagesBootPath + ASConfig::GetValue(key);
	else return NULL;
}

string ASConfigTheme::GetValue(string key)
{
	pspDebugScreenPrintf(ASConfig::GetValue(key).c_str());
	return ASConfig::GetValue(key);
}

void ASConfigTheme::SetValue(string key, string value)
{
	ASConfig::SetValue(key, value);
}
