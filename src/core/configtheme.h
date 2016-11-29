#ifndef CONFIGTHEME_H
#define CONFIGTHEME_H

#include "artillery.h"

class ConfigTheme : public Config {
	public:
		ConfigTheme(string themeName);
		string GetPathValue(string key);
		string GetValue(string key);
		void SetValue(string key, string value);
		
	public:
		string ImagesCorePath;
		string ImagesMP3Path;
		string ImagesIconPath;
		string ImagesBootPath;
		string IconConfig;
		string ThemeDir;
};
#endif
