/*
# PSP Framework 				
# ------------------------------------------------------------------
# Author: R.P.Heuts
# Date: 19-06-2006
# Version: 0.1
# 
# Class: Config
# Desciption:
# Provides functions for handling config files. The class should be
# used as a base class, so if specific functions are required it
# should be inherited by a class that will provide it.
*/

#include "ASIncludes.h"
#include <stdio.h>

ASConfig::ASConfig(string file)
{
	configFile = file;
	configFileOpen = 0;
	configFileBuffered = 0;
}

ASConfig::~ASConfig()
{
	CloseConfigFile();
}

string ASConfig::GetValue(string key)
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

void ASConfig::SetValue(string key, string value)
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

bool ASConfig::CheckValue(string key)
{
	if (GetValue(key).find("1") == string::npos)
		return false;
	else
		return true;
}

bool ASConfig::GetIntValue(string key, int &r)
{
	const char* d = GetValue(key).c_str();
	for(r=0;*d>='0'&&*d<='9';r=r*10+*(d++)-'0');return*d==0;
}

int ASConfig::GetIntValue(string key)
{
	int ret = 0;
	GetIntValue(key, ret);
	return ret;
}

void ASConfig::CloseConfig()
{
	WriteFullConfigFile();
}

void ASConfig::OpenConfigFile()
{
	
	if(!(pFile = fopen(configFile.c_str(), "r")))
   		return;
 	configFileOpen = 1;
}

void ASConfig::OpenConfigFileWrite()
{
	
	if(!(pFile = fopen(configFile.c_str(), "w")))
		return;
	configFileOpen = 1;
}

void ASConfig::CloseConfigFile()
{
	if (configFileOpen == 1)
		fclose(pFile);
 	configFileOpen = 0;
}

void ASConfig::ReadFullConfigFile()
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

void ASConfig::WriteFullConfigFile()
{
	if(configFileOpen == 0)
		OpenConfigFileWrite();
	if(configFileOpen == 0)
		return;
	
	fputs(configFileBuffer.c_str(), pFile);
	CloseConfigFile();
}
