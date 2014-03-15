#ifndef ASCONFIGTHEME_H
#define ASCONFIGTHEME_H

#include "ASIncludes.h"

class ASConfigTheme : public ASConfig {
	public:
		ASConfigTheme();
		string GetPathValue(string key);
		string GetValue(string key);
		void SetValue(string key, string value);
		
	public:
		string ImagesCorePath;
		string ImagesBootPath;
		string IconConfig;
		string ThemeDir;
};
#endif
