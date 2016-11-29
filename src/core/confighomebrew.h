#ifndef CONFIGHOMEBREW_H
#define CONFIGHOMEBREW_H

#include "artillery.h"
#include <list>

class ConfigHomebrew : private Config {
	public:
		ConfigHomebrew(string homebrewConfig);
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
