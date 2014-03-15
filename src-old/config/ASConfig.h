#ifndef ASCONFIG_H
#define ASCONFIG_H

#include "ASIncludes.h"

class ASConfig {
	public:
		ASConfig(string file);
		~ASConfig();
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
