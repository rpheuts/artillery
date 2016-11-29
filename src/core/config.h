#ifndef CONFIG_H
#define CONFIG_H

#include "artillery.h"

class Config {
	public:
		Config(string file);
		~Config();
		string GetValue(string key);
		void SetValue(string key, string value);
		void GetValue(string key, string value);
		bool CheckValue(string key);
		bool GetIntValue(string key, int &r);
		int GetIntValue(string key);
		void CloseConfig();
	
	protected:
		void OpenConfigFile();
		void OpenConfigFileWrite();
		void ReadFullConfigFile();
		void WriteFullConfigFile();
		void CloseConfigFile();

	protected:
		string configFile;
		string configFileBuffer;
		int configFileOpen;
		int configFileBuffered;
		FILE* pFile;
};
#endif
