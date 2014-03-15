#ifndef ASCONFIGHOMEBREW_H
#define ASCONFIGHOMEBREW_H

#include "ASIncludes.h"
#include <list>

class ASConfigIcons : private ASConfig {
	public:
		ASConfigIcons(string iconConfig);
		string GetIconValue(string name);
		string GetIconSelValue(string name);
		string GetPathValue(string name);
		string GetPathPerValue(string name);
		int GetPosXValue(string name);
		int GetPosYValue(string name);
		bool CheckDesktopValue(string name);
		void FillIconList(list<string>* iconList);

	private:
		void SetBuffer(string name);
		
	private:
		string _configPath;
		string _fileBuffer;
};
#endif
