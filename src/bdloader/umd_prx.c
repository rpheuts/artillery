#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <pspsdk.h>
#include <string.h>
#include <stdio.h>

#include <pspctrl.h>
#include <pspdebug.h>
#include <pspumd.h>

#define MODULE_NAME		"UMD_LOADER"
#define VERSION 		"0.2"
#define REBOOT_TIME 		6000000

#define printf	pspDebugScreenPrintf

PSP_MODULE_INFO(MODULE_NAME, 0x1000, 1, 1);
/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(0);

static char * rebootPath;
static char sElfPath[255];
static int runELF = 0;
int done = 0;

//////////////////////////////////////////////////////////////////////
// prototypes
//////////////////////////////////////////////////////////////////////

int unload_loader(char * moduleName);
void rebootApp(void);
int run_umd(char * elfPath);
void crash_handler(PspDebugRegBlock *regs);
int unload_loader(char * moduleName);
int mount_umd(void);
int run_elf();
void writeByte(int fd, unsigned char data);
int screenshot();
char * get_Vram_Addr(unsigned long x,unsigned long y, long frame);

//////////////////////////////////////////////////////////////////////
// Reboot entry point
//////////////////////////////////////////////////////////////////////
void rebootApp(void)
{

	done = 1;

	struct SceKernelLoadExecParam execParam;

	execParam.size = sizeof(execParam);
	execParam.argp = rebootPath;
	execParam.args = strlen(rebootPath);
	execParam.key = NULL;

	sceKernelLoadExec(rebootPath, &execParam);

}

//////////////////////////////////////////////////////////////////////
// Load module
//////////////////////////////////////////////////////////////////////
SceUID load_module(const char *path, int flags, int type)
{
	SceKernelLMOption option;
	SceUID mpid;

	/* If the type is 0, then load the module in the kernel partition, otherwise load it
	   in the user partition. */
	if (type == 0)
	{
		mpid = 1;
	}
	else
	{
		mpid = 2;
	}

	memset(&option, 0, sizeof(option));
	option.size = sizeof(option);
	option.mpidtext = mpid;
	option.mpiddata = mpid;
	option.position = 0;
	option.access = 1;

	return sceKernelLoadModule(path, flags, type > 0 ? &option : NULL);
}

void runUMD()
{
	//Mount UMD
	int res_mount;
	res_mount = mount_umd();

	// Unload bootloader module
	int res_unload = unload_loader( "BOOTLOADER" );

	printf("[MbShell] Running UMD...\n");
	//if successfully removed, run the UMD
	if(res_unload != -1)
	{
		int res_run = run_umd( "disc0:/PSP_GAME/SYSDIR/BOOT.BIN" );
		if(res_run == -1)
		{
			printf("[MbShell] Cannot run umd...\n");
			crash_handler(0);
		}
	}
	else
	{
		printf("[MbShell] Cannot unload bootloader...\n");
		crash_handler(0);
	}
}

//////////////////////////////////////////////////////////////////////
// Thread
//////////////////////////////////////////////////////////////////////
int main_thread(SceSize args, void *argp)
{
	pspDebugScreenInit();
	// Provide ELF name for reboot patch
	rebootPath = "ms0:/MbShell/Artillery.elf";
	
	if (strstr(sElfPath, "Artillery.elf"))
		rebootApp();
	//Install patchs to allow Plain check module from MS0 and a crash handler
	pspSdkInstallNoDeviceCheckPatch();
	pspSdkInstallNoPlainModuleCheckPatch();
	pspDebugInstallErrorHandler(crash_handler);

	// patch the kernel@sceGameExit to restart shell !
	#define J(addr)       (0x08000000 | (0xFFFFFFF & (((u32)(addr)) >> 2)))
	*((unsigned int *)0x8806882c) = J(rebootApp);
	*((unsigned int *)0x88068830) = 0x00000000; //  nop
	
	if (runELF)
	{
		if (run_elf() == -1);
		{
			//sceKernelDelayThread(10000000);
			//rebootApp();
		}
	}
	else
		runUMD();

	if (!strstr(sElfPath, "Artillery.elf"))
		sceKernelSelfStopUnloadModule(0, 0, NULL);
	sceKernelExitDeleteThread(0);
	return 0;

}

//////////////////////////////////////////////////////////////////////
// Module Entry Point
//////////////////////////////////////////////////////////////////////
int main(int args, char **argp)
{
	int thid;
	u32 func;

	// Relocate main thread below normal 0x89000000 to avoid data crunching
	func = (u32) main_thread;
	func |= 0x80000000;

	/*
	thid = sceKernelCreateThread("main_thread", main_thread, 0x19, 0x10000, 0, NULL);
	if(thid >= 0)
	{
		//Provide entry points args to the thread
		sceKernelStartThread(thid, args, argp);
	}
	*/

	if (args > 0)
	{
		strcpy(sElfPath, argp[0]);
		runELF = 1;
	}

	/* Create a high priority thread */
	thid = sceKernelCreateThread("main_thread", (void *) func, 0x20, 0x10000, 0, NULL);
	if(thid >= 0)
	{
		//Provide entry points args to the thread
		sceKernelStartThread(thid, args, argp[0]);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Module Exit Point
//////////////////////////////////////////////////////////////////////
int module_stop(SceSize args, void *argp)
{
	//sceKernelExitDeleteThread(0);

	return 0;
}

int run_elf()
{
	SceUID modid_loaded, modid_started;
	
	// Unload bootloader module
	unload_loader( "BOOTLOADER" );

	printf("[MbShell] Running ELF...\n");
	//if successfully removed, run the ELF

	// Load module
	modid_loaded = load_module( sElfPath, 0, 0);

	// Start module with input path
	if(modid_loaded >= 0)
	{
		int ret_val = 1;
		modid_started = sceKernelStartModule(modid_loaded, strlen( sElfPath ), sElfPath, &ret_val, NULL);

		if(modid_started < 0)
		{
			printf("[ELF Loader] Error starting elf (%08X)\n", modid_started);
			return -1;
		}
		#ifdef DEBUG
		{
			printf("[ELF Loader] ELF module %08X started\n", modid_started);
		}
		#endif

	}
	else
	{
		printf("[ELF Loader] Error loading elf %08X\n", modid_loaded);
		return -1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// UMD Loader
//////////////////////////////////////////////////////////////////////
int run_umd(char * umdPath)
{
	SceUID modid_loaded, modid_started;

	// Load module
	modid_loaded = load_module( umdPath, 0, 0);

	#ifdef DEBUG
	printf("[MbShell] Module ID %08X Loaded\n", modid_loaded);
	#endif

	// Start module with input path
	if(modid_loaded >= 0)
	{
		int ret_val = 1;

		modid_started = sceKernelStartModule(modid_loaded, strlen( umdPath ) + 1, umdPath, &ret_val, NULL);

		if(modid_started < 0)
		{
			printf("[MbShell] Error starting UMD (%08X)\n", modid_started);
			return -1;
		}
		#ifdef DEBUG
		{
			printf("[MbShell] UMD module %08X started\n", modid_started);
		}
		#endif

	}
	else
	{
		printf("[MbShell] Error loading UMD %08X\n", modid_loaded);
		return -1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Unload bootloader routine
//////////////////////////////////////////////////////////////////////
int unload_loader(char * moduleName)
{
	SceModule *mod;
	SceUID modid;
	int ret_stop, ret_unload = 0;
	int status;

	#ifdef DEBUG
	printf("\n[MbShell] Unloading bootloader : '%s'\n", moduleName);
	#endif

	mod = sceKernelFindModuleByName(moduleName);
	if(mod != NULL)
	{
		#ifdef DEBUG
		printf("[MbShell] Loader UID: %08X\n", mod->modid);
		#endif

		/* Stop module */
		modid = mod->modid;

		ret_stop = sceKernelStopModule(modid, 0, NULL, &status, NULL);
		if(ret_stop >= 0)
		{
			ret_unload = sceKernelUnloadModule(modid);
			if(ret_unload < 0)
			{
				printf("[MbShell] Couldn't Unload bootloader %s\n", moduleName);
				return -1;
			}
		}
		else
		{
			printf("[MbShell] Couldn't Stop bootloader %s\n", moduleName);
			return -1;
		}
	}
	else
	{
		printf("[MbShell] Couldn't Find bootloader %s\n", moduleName);
		return -1;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Mount UMD
//////////////////////////////////////////////////////////////////////
int mount_umd(void)
{
	int res;

	res = sceUmdCheckMedium(0);
	if(res == 0)
	{
		printf("\n[MbShell] Insert an UMD\n");
		res = sceUmdWaitDriveStat(UMD_WAITFORDISC);
	}

	res = sceUmdActivate(1, "disc0:");

	#ifdef DEBUG
	printf("\n[MbShell] Mounted disc\n");
	#endif

	res = sceUmdWaitDriveStat(UMD_WAITFORINIT);

	return res;
}


//////////////////////////////////////////////////////////////////////
// Crash routine
//////////////////////////////////////////////////////////////////////
void crash_handler(PspDebugRegBlock *regs)
{
	pspDebugScreenSetTextColor(0xFF);
	printf("\n\[MbShell] Error\n Returning to MbShell...");

	sceKernelDelayThread(REBOOT_TIME); // Delay for REBOOT_TIME seconds

	// Reboot caller !
	rebootApp();
}

