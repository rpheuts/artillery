#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include "artillery.h"
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdebug.h>

#include <pspiofilemgr.h>
#include <pspiofilemgr_fcntl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void (*STATUS_PROC)(int percent, void* statusData);

class WiFiManager
{
	public:
		static int Init();
		static void Connect();
		static int IsConnected();
		static int Terminate();
		static int RecvBlockTillDone(SOCKET s, u8* buf, int len);
		static int SetSockNoBlock(SOCKET s, u32 val);
		static SOCKET SimpleConnectWithTimeout(int sockType, const u8 ip_addr[4], u16 port, int timeoutTicks, STATUS_PROC statusProc, void* statusData);
		
	private:
		static unsigned short htons(unsigned short wIn);
		static int _connected;
};

#endif
