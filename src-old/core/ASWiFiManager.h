#ifndef ASWIFIMANAGER_H
#define ASWIFIMANAGER_H

#include "ASIncludes.h"

class ASWiFiManager
{
	public:
		ASWiFiManager();
		~ASWiFiManager();
		int Connect();
		int Disconnect();
		bool Connected();
		
	private:
		bool _connected;
};

#endif
