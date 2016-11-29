#include "Includes.h"

Config::Config(File* file)
{
	configFile = file;
	configFileBuffered = 0;
}

Config::~Config()
{
	delete (configFile);	
}

string Config::GetValue(string key)
{
	if (configFileBuffered == 0)
		ReadFullConfigFile();
	
	if (configFileBuffered == 1)
	{
		string::size_type linestart = configFileBuffer.find(key + " ", 0);
		if (linestart == string::npos)
			return "";
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

void Config::SaveConfig()
{
	WriteFullConfigFile();
}

bool Config::CheckValue(string key)
{
	if (GetValue(key).find("1") == string::npos)
		return false;
	else
		return true;
}

int Config::GetIntValue(string key)
{
	int ret = 0;
	GetIntValue(key, ret);
	return ret;
}

void Config::ReadFullConfigFile()
{
	FILE* fp;
	if (!(fp = configFile->Open("r")))
		return;
	    
	char buffer[256];
	while(fgets(buffer, 256, fp) != NULL)
	{
		configFileBuffer.append(buffer);
	}
	
	configFileBuffered = 1;
	configFile->Close();
}

void Config::WriteFullConfigFile()
{
	FILE* fp;
	if (!(fp = configFile->Open("w")))
		return;
	
	fputs(configFileBuffer.c_str(), fp);
	configFile->Close();
}

bool Config::GetIntValue(string key, int &r)
{
	string val = GetValue(key);
	const char* d = val.c_str();
	for(r=0;*d>='0'&&*d<='9';r=r*10+*(d++)-'0'); return*d==0;
}
