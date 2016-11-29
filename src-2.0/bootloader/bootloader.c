#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspsdk.h>
#include <string.h>
#include <pspusb.h>
#include <pspusbstor.h>

PSP_MODULE_INFO("Artillery-Boot", 0x1000, 1, 1);
/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(0);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

// Define the modules to be started
#define SHELL_MODULE_COUNT 1
char *shellModules[SHELL_MODULE_COUNT] = {"ms0:/Artillery/modules/core.prx"};
#define KERNEL_MODULE_COUNT 5
char *kernModules[KERNEL_MODULE_COUNT] = {"flash0:/kd/semawm.prx", "flash0:/kd/usbstor.prx", "flash0:/kd/usbstormgr.prx", "flash0:/kd/usbstorms.prx", "flash0:/kd/usbstorboot.prx"};

// Prototypes
void InitModules();
void InitUsb();
void ErrorHandler(const char* errorMsg, int critical);
void DoneHandler();
SceUID LoadStartCustomModule(const char* path);

/* Exit callback */
int exit_callback(int arg1, int arg2, void *arg)
{
	sceKernelExitGame();

	return 0;
}

/* Callback thread */
void CallbackThread(void *arg)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", (void*) CallbackThread, 0x11, 0xFA0, 0xa0000000, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

__attribute__ ((constructor))
void loaderInit()
{
    pspKernelSetKernelPC();
    pspSdkInstallNoDeviceCheckPatch();
}

int main(int argc, char** argv)
{
	// Init the console
	pspDebugScreenInit();
	pspDebugScreenClear();
	
	printf("Artillery Shell - Bootloader v0.2\n");
	printf("--------------------------------------------\n\n");
	
	printf("Setting up environment...");
	pspDebugInstallErrorHandler(NULL);
	pspDebugInstallStdoutHandler(pspDebugScreenPrintData);
	pspSdkInstallNoPlainModuleCheckPatch();
	SetupCallbacks();
	DoneHandler();
	
	// Setup environment
	InitModules();

	// End this thread
	sceKernelExitDeleteThread(0);
	return 0;
}

void InitModules()
{
	SceUID uid;
	int i = 0;
	int ret = 0;
	
	printf("Loading Kernel modules...\n");
	for (i=0; i<KERNEL_MODULE_COUNT; i++)
	{
		printf("Loading module: %s", kernModules[i]);
		uid = pspSdkLoadStartModule(kernModules[i], 0);
		
		if(uid < 0)
			ErrorHandler("Failed to load kernel modules", 1);
		else
			DoneHandler();	
	}

	printf("Loading WiFi modules...");
	ret = pspSdkLoadInetModules();  
	if (ret != 0)
		ErrorHandler("Failed to load WiFi modules", 0);
	else
		DoneHandler();
	
	printf("Loading USB modules...\n");
	InitUsb();
	
	printf("Loading Shell modules...\n");
	for (i=0; i<SHELL_MODULE_COUNT; i++)
	{
		printf("Loading module: %s", shellModules[i]);
		uid = LoadStartCustomModule(shellModules[i]);

		if(uid < 0)
		{
			pspDebugScreenPrintf("   Error: %x\n", uid);
			ErrorHandler("Failed to load shell modules", 1);
		}
		else
			DoneHandler();
	}

	ret = 0;
}

void InitUsb()
{
	int retVal;

    	//setup USB drivers
	printf("Loading module: %s", PSP_USBBUS_DRIVERNAME);
	retVal = sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
	if (retVal != 0)
	{
		printf("Error starting USB Bus driver (0x%08X)\n", retVal);
		sceKernelSleepThread();
	}
	else
		DoneHandler();

	printf("Loading module: %s", PSP_USBSTOR_DRIVERNAME);
	retVal = sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
	if (retVal != 0)
	{
		printf("Error starting USB Mass Storage driver (0x%08X)\n", retVal);
		sceKernelSleepThread();
	}
	else
		DoneHandler();

	printf("Setting up USB...");
	retVal = sceUsbstorBootSetCapacity(0x800000);
	if (retVal != 0)
	{
		printf("Error setting capacity with USB Mass Storage driver (0x%08X)\n", retVal);
		sceKernelSleepThread();
	}
	else
		DoneHandler();
	
	retVal = 0;
}

SceUID LoadStartCustomModule(const char* path)
{
	SceKernelLMOption option;
	SceUID mpid= 2;
	SceUID modid;

	memset(&option, 0, sizeof(option));
	option.size = sizeof(option);
	option.mpidtext = mpid;
	option.mpiddata = mpid;
	option.position = 0;
	option.access = 1;

	modid = sceKernelLoadModule(path, 0, &option);
	if (modid > 0)
	{
		int fd;
		return sceKernelStartModule(modid, 0, NULL, &fd, NULL);
	}
	else
		return modid;
}

void ErrorHandler(const char* errorMsg, int critical)
{
	// Print failed to the screen
	pspDebugScreenSetXY (60, pspDebugScreenGetY());
	pspDebugScreenSetTextColor(0x000000FF);
	printf("[Failed]");
	
	if (critical == 1)
	{
		// Criticial error, notify user and exit
		printf("\nCritical error, exiting in 10 seconds...\n");
		sceKernelDelayThread(10000000);
		sceKernelExitGame();
		sceKernelExitDeleteThread(0);
	}
	pspDebugScreenSetTextColor(0xFFFFFFFF);
}

void DoneHandler()
{
	// Print ok to the screen
	pspDebugScreenSetXY (64, pspDebugScreenGetY());
	pspDebugScreenSetTextColor(0x0000FF00);
	printf("[Ok]");
	pspDebugScreenSetTextColor(0xFFFFFFFF);
}
