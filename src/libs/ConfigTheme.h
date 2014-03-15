#ifndef CONFIGTHEME_H
#define CONFIGTHEME_H

#include "Includes.h"

class ConfigTheme : public Config
{
	public:
		ConfigTheme();
		string GetPathValue(string key);
		string GetValue(string key);
		void SetValue(string key, string value);
		
	public:
		string ImagesCorePath;
		string ImagesBootPath;
		string ThemeDir;
};
#endif
