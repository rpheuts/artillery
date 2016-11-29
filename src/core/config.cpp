#include "artillery.h"
#include <stdio.h>

Config::Config(string file)
{
	configFile = file;
	configFileOpen = 0;
	configFileBuffered = 0;
}

Config::~Config()
{
	CloseConfigFile();
}

string Config::GetValue(string key)
{
	if (configFileBuffered == 0)
		ReadFullConfigFile();
	
	if (configFileBuffered == 1)
	{
		string::size_type linestart = configFileBuffer.find(key+" ", 0);
		string::size_type valuestart = configFileBuffer.find("=", linestart) + 2;
		string::size_type lineend = configFileBuffer.find(";", linestart);
		
		string ret =  configFileBuffer.substr(valuestart, lineend - valuestart);
		return ret;
	}
	return "";
}

void Config::SetValue(string key, string value)
{
	if (configFileBuffered == 0)
		ReadFullConfigFile();
	
	if (configFileBuffered == 1)
	{
		string::size_type linestart = configFileBuffer.find(key+" ", 0);
		string::size_type valuestart = configFileBuffer.find("=", linestart) + 2;
		string::size_type lineend = configFileBuffer.find(";", linestart);
		
		configFileBuffer.erase(valuestart, lineend-valuestart);
		configFileBuffer.insert(valuestart, value);
	}
}

bool Config::CheckValue(string key)
{
	if (GetValue(key).find("1") == string::npos)
		return false;
	else
		return true;
}

bool Config::GetIntValue(string key, int &r)
{
	const char* d = GetValue(key).c_str();
	for(r=0;*d>='0'&&*d<='9';r=r*10+*(d++)-'0');return*d==0;
}

int Config::GetIntValue(string key)
{
	int ret = 0;
	GetIntValue(key, ret);
	return ret;
}

void Config::CloseConfig()
{
	WriteFullConfigFile();
}

void Config::OpenConfigFile()
{
	
	if(!(pFile = fopen(configFile.c_str(), "r")))
   		return;
 	configFileOpen = 1;
}

void Config::OpenConfigFileWrite()
{
	
	if(!(pFile = fopen(configFile.c_str(), "w")))
		return;
	configFileOpen = 1;
}

void Config::CloseConfigFile()
{
	if (configFileOpen == 1)
		fclose(pFile);
 	configFileOpen = 0;
}

void Config::ReadFullConfigFile()
{
	if(configFileOpen == 0)
		OpenConfigFile();
	if(configFileOpen == 0)
		return;
	
	char buffer[256];
	while(fgets(buffer, 256, pFile) != NULL)
	{
		configFileBuffer.append(buffer);
	}
	
	configFileBuffered = 1;
	CloseConfigFile();
}

void Config::WriteFullConfigFile()
{
	if(configFileOpen == 0)
		OpenConfigFileWrite();
	if(configFileOpen == 0)
		return;
	
	fputs(configFileBuffer.c_str(), pFile);
	CloseConfigFile();
}
