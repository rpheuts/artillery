#ifndef CONFIG_H
#define CONFIG_H

#include "Includes.h"

class Config {
	public:
		Config(File* file);
		~Config();
		string GetValue(string key);
		void SetValue(string key, string value);
		bool CheckValue(string key);
		int GetIntValue(string key);
		bool GetIntValue(string key, int &r);
		void SaveConfig();
	
	protected:
		void ReadFullConfigFile();
		void WriteFullConfigFile();

	protected:
		File* configFile;
		string configFileBuffer;
		int configFileBuffered;
};
#endif
