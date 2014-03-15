#include "ASIncludes.h"
#include <stdio.h>

ASConfigIcons::ASConfigIcons(string homebrewConfig) : ASConfig("empty")
{
	_configPath = homebrewConfig;
	
	// Read in config file
	if(!(pFile = fopen(homebrewConfig.c_str(), "r")))
		return;
	char buffer[256];
	while(fgets(buffer, 256, pFile) != NULL)
		_fileBuffer.append(buffer);
	fclose(pFile);
}

void ASConfigIcons::SetBuffer(string name)
{
	string::size_type linestart = _fileBuffer.find("<" + name, 0);
	string::size_type lineend = _fileBuffer.find(">", linestart);
	configFileBuffer = _fileBuffer.substr(linestart, lineend - linestart);
	configFileBuffered = 1;
}

string ASConfigIcons::GetIconValue(string name)
{
	SetBuffer(name);
	return GetValue("icon");
}

string ASConfigIcons::GetIconSelValue(string name)
{
	SetBuffer(name);
	return GetValue("iconsel");
}

string ASConfigIcons::GetPathValue(string name)
{
	SetBuffer(name);
	return GetValue("path");
}

string ASConfigIcons::GetPathPerValue(string name)
{
	SetBuffer(name);
	return GetValue("path%");
}

int ASConfigIcons::GetPosXValue(string name)
{
	SetBuffer(name);
	int ret = 0;
	GetIntValue("posx", ret);
	return ret;
}

int ASConfigIcons::GetPosYValue(string name)
{
	SetBuffer(name);
	int ret = 0;
	GetIntValue("posy", ret);
	return ret;
}

bool ASConfigIcons::CheckDesktopValue(string name)
{
	SetBuffer(name);
	return CheckValue("desktop");
}

void ASConfigIcons::FillIconList(list<string>* iconList)
{
	string::size_type current = _fileBuffer.find("<", 0);
	string::size_type end = 0;
	if (current == string::npos)
		return;
	
	while (current != string::npos)
	{
		end = _fileBuffer.find(";", current);
		iconList->push_back(_fileBuffer.substr(current + 1, end - current - 1));
		current = _fileBuffer.find("<", end);
	}
}
