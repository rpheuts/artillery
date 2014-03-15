#include "ASIncludes.h"
#include <pspwlan.h>

ASWiFiManager::ASWiFiManager()
{
	_connected = false;
}

ASWiFiManager::~ASWiFiManager()
{
	
}

int ASWiFiManager::Connect()
{
	return 0;
}

int ASWiFiManager::Disconnect()
{
	return 0;
}

bool ASWiFiManager::Connected()
{
	return _connected;
}
