#include "artillery.h"
#include <pspwlan.h>

#define false 0
#define true 1

int WiFiManager::_connected = 0;

//UserdataStubs(Socket, Socket*)

extern "C" {
// general init
int sceNetInit(u32 r4, u32 r5, u32 r6, u32 r7, u32 r8);
int sceNetTerm();
// internet
int sceNetResolverInit();
int sceNetApctlInit(u32 r4, u32 r5);
int sceNetApctlTerm();
int sceNetResolverTerm();
int sceNetInetInit();
int sceNetInetTerm();
int sceUtilityCheckNetParam(int iConfig);
int sceUtilityGetNetParam(int iConfig, u32 r5_gettype, void* r6return);
int sceNetApctlConnect(int connect_index); // 0 is special
int sceNetApctlGetState(int* state_ptr);
int sceNetApctlGetInfo(int code, void* data);
int sceNetApctlDisconnect();
}

int WiFiManager::Init()
{
    u32 err;
    err = sceNetInit(0x20000, 0x20, 0x1000, 0x20, 0x1000);
    if (err != 0)
        return err;
    err = sceNetInetInit();
    if (err != 0)
        return err;
    err = sceNetResolverInit();
    if (err != 0)
        return err;
    err = sceNetApctlInit(0x1000, 0x42);
    if (err != 0)
        return err;
    return 0;   // it worked!
}

int WiFiManager::Terminate()
{
    u32 err;

    err = sceNetApctlTerm();
    if (err == 0x80410A04)
        Console::Print("sceNetApctlTerm returned common error");
    else if (err != 0)
        Console::Print("sceNetApctlTerm returns error");

    err = sceNetResolverTerm();
    if (err != 0)
        Console::Print("sceNetResolverTerm returns error");

    err = sceNetInetTerm();
    if (err != 0)
        Console::Print("sceNetInetTerm returns error");
    err = sceNetTerm();
    if (err != 0)
        Console::Print("sceNetTerm returns error");
       
	_connected = false;
	LCD::PrintMessage("WIFI Disconnected");
   return 0; // assume it worked
}

void WiFiManager::Connect()
{
	if (IsConnected())
		return;
	if (!sceWlanGetSwitchState())
	{
		LCD::PrintMessage("WiFi not enabled");
		return;
	}
	
	Console::Print("Initializing WiFi...");
	int err = WiFiManager::Init();
	if (err != 0)
 	{
 		Console::Print("Error, bailing out...");
 		Terminate();
 		return;
 	}
 	
 	#define MAX_PICK 10
    struct
    {
        int index;
        char name[64];
    } picks[MAX_PICK];
    int pick_count = 0;

    int iNetIndex;
    for (iNetIndex = 1; iNetIndex < 100; iNetIndex++) // skip the 0th connection
    {
        if (sceUtilityCheckNetParam(iNetIndex) != 0)
            break;  // no more
        sceUtilityGetNetParam(iNetIndex, 0, picks[pick_count].name);
        picks[pick_count].index = iNetIndex;
        pick_count++;
        if (pick_count >= MAX_PICK)
            break;  // no more room
    }
    if (pick_count == 0)
    {
        Console::Print("No connections");
        LCD::PrintMessage("WIFI: No connections");
        Terminate();
        return;
    }
    Console::Print("Found connections");
    
    int connectionConfig = picks[0].index;


    // Connect
    err = sceNetApctlConnect(connectionConfig);
    if (err != 0)
    {
       Console::Print("sceNetApctlConnect returns error");
       Terminate();
       return;
    }

    // Report status while waiting for connection to access point
    int stateLast = -1;
    Console::Print("Connecting...");
    while (1)
    {
        int state;
        err = sceNetApctlGetState(&state);
        if (err != 0)
        {
            Console::Print("sceNetApctlGetState returns error");
				return;
        }
        if (state > stateLast)
        {
            Console::Print("Next state...");
            stateLast = state;
        }
        if (state == 4)
            break;  // connected with static IP
        sceKernelDelayThread(50*1000); // 50ms
    }
    Console::Print("Connected!");
    LCD::PrintMessage("WIFI Connected");
    _connected = true;
}

int WiFiManager::IsConnected()
{
	return _connected;
}

////////////////////////////////////////////////////////////////////

// byte swap - REVIEW: is there a helper in the NetLib ?
unsigned short WiFiManager::htons(unsigned short wIn)
{
    u8 bHi = (wIn >> 8) & 0xFF;
    u8 bLo = wIn & 0xFF;
    return ((unsigned short)bLo << 8) | bHi;
}

int WiFiManager::RecvBlockTillDone(SOCKET s, u8* buf, int len)
{
    // 'sceNetInetRecv' may return partial results
    //  call repeatedly until entire receive buffer is full
    int nTotal = 0;
    while (len > 0)
    {
        int n = sceNetInetRecv(s, buf, len, 0); // blocking for anything
        if (n <= 0)
            return n; // error
        nTotal += n;
        buf += n;
        len -= n;
    }
    return nTotal;
}

int WiFiManager::SetSockNoBlock(SOCKET s, u32 val)
{ 
    return sceNetInetSetsockopt(s, SOL_SOCKET, 0x1009, (const char*)&val, sizeof(u32));
}

SOCKET WiFiManager::SimpleConnectWithTimeout(int sockType, const u8 ip_addr[4], u16 port, int timeoutTicks, STATUS_PROC statusProc, void* statusData)
{
    SOCKET sock;
    struct sockaddr_in addrTo;
    uint err;

    sock = sceNetInetSocket(AF_INET, sockType, 0);
    if (sock & 0x80000000)
        return INVALID_SOCKET;

    addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(port);
    addrTo.sin_addr[0] = ip_addr[0];
    addrTo.sin_addr[1] = ip_addr[1];
    addrTo.sin_addr[2] = ip_addr[2];
    addrTo.sin_addr[3] = ip_addr[3];

    // with regular blocking settings, the PSP wedges waiting for a connection
    // if the IP is not valid
    // instead we start a non-blocking connect, and check the status waiting
    // for a connect

    SetSockNoBlock(sock, 1);

    err = sceNetInetConnect(sock, &addrTo, sizeof(addrTo));
    // start the process

    if (err == 0)
    {
        // surprising - it worked immediately
        SetSockNoBlock(sock, 0); // turn blocking back on for recv
        return sock;    // worked immediately
    }

    if (err == 0xFFFFFFFF && sceNetInetGetErrno() == 0x77)
    {
        // connect process started, not yet complete
		  #define TICKS_PER_POLL 20
        int ticks;
        for (ticks = 0; ticks < timeoutTicks; ticks += TICKS_PER_POLL)
        {
            // try connect again, which should always fail
            // look at why it failed to figure out if it is connected
            //REVIEW: a conceptually cleaner way to poll this?? (accept?)
            err = sceNetInetConnect(sock, &addrTo, sizeof(addrTo));
            if (err == 0 || (err == 0xFFFFFFFF && sceNetInetGetErrno() == 0x7F))
            {
                // now connected - I hope
                SetSockNoBlock(sock, 0); // turn blocking back on for recv
                return sock;
            }
            if (statusProc != NULL)
                (*statusProc)(100 * ticks / timeoutTicks, statusData);
            sceKernelDelayThread(TICKS_PER_POLL*1000);
        }
    }

    // something wrong - general error or timeout on connect
    //REVIEW: sceNetInetSocketAbort(sock); ???
    sceNetInetClose(sock);

    return INVALID_SOCKET;
}
