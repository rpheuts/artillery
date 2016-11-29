#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <pspsdk.h>
#include <string.h>
#include <stdio.h>
#include <pspdebug.h>
#include <string.h>
#include <pspumd.h>
#include <malloc.h>

#define MODULE_NAME		"BACKDOOR"
#define VERSION 		"0.1a"

#define printf	pspDebugScreenPrintf
PSP_MODULE_INFO(MODULE_NAME, 0x1000, 1, 1);
/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(0);

typedef struct
{
	char execPath[255];
	char shellPath[255];
	char mInterface[10];
	char mAudio[10];
} LauncherInfo;

static LauncherInfo* lInfo;
static char * rebootPath;


//////////////////////////////////////////////////////////////////////
// Reboot entry point
//////////////////////////////////////////////////////////////////////
void rebootApp(void)
{
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

//////////////////////////////////////////////////////////////////////
// Unload bootloader routine
//////////////////////////////////////////////////////////////////////
int unload_loader(char * moduleName)
{
	SceModule *mod;
	SceUID modid;
	int ret_stop, ret_unload = 0;
	int status;

	mod = sceKernelFindModuleByName(moduleName);
	if(mod != NULL)
	{
		#ifdef DEBUG
		printf("[ELF Loader] Loader UID: %08X\n", mod->modid);
		#endif

		/* Stop module */
		modid = mod->modid;

		ret_stop = sceKernelStopModule(modid, 0, NULL, &status, NULL);
		if(ret_stop >= 0)
		{
			ret_unload = sceKernelUnloadModule(modid);
			if(ret_unload < 0)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}


void bdPatchKernel()
{
	// patch the kernel@sceGameExit to restart shell !
	#define J(addr)       (0x08000000 | (0xFFFFFFF & (((u32)(addr)) >> 2)))
	*((unsigned int *)0x8806882c) = J(rebootApp);
	*((unsigned int *)0x88068830) = 0x00000000; //  nop
}

int mount_umd(void)
{
	int res;

	res = sceUmdCheckMedium(0);
	if(res == 0)
	{
		printf("\n[UMD Loader] Insert an UMD\n");
		res = sceUmdWaitDriveStat(UMD_WAITFORDISC);
	}

	res = sceUmdActivate(1, "disc0:");

	#ifdef DEBUG
	printf("\n[UMD Loader] Mounted disc\n");
	#endif

	res = sceUmdWaitDriveStat(UMD_WAITFORINIT);

	return res;
}

int main_thread(SceSize args, void *argp)
{
	printf("Unloading MbShell modules...");
	
	// Unload shell modules
	unload_loader( lInfo->mInterface );
	unload_loader( lInfo->mAudio );

	printf("Mounting UMD...");
	//Mount UMD
	int res_mount;
	res_mount = mount_umd();

	SceUID modid_loaded, modid_started;

	// Load module
	modid_loaded = load_module( lInfo->execPath, 0, 0);

	// Start module with input path
	if(modid_loaded >= 0)
	{
		int ret_val = 1;
		modid_started = sceKernelStartModule(modid_loaded, strlen( lInfo->execPath ) + 1, lInfo->execPath, &ret_val, NULL);
	}

	/*
	// Unload shell modules
	unload_loader( lInfo->mInterface );
	unload_loader( lInfo->mAudio );

	SceUID modid_loaded, modid_started;

	// Load module
	modid_loaded = load_module(lInfo->execPath, 0, 0);

	// Start module with input path
	if(modid_loaded >= 0)
	{
		int ret_val = 1;
		modid_started = sceKernelStartModule(modid_loaded, strlen(lInfo->execPath) + 1, lInfo->execPath, &ret_val, NULL);
	}
	launch_elf = 0;
	free(lInfo);*/
	return 0;
}

void crash_handler(PspDebugRegBlock *regs)
{
	// Reboot caller !
	rebootApp();
}

//////////////////////////////////////////////////////////////////////
// Module Entry Point
//////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	lInfo = (LauncherInfo*) malloc(sizeof(LauncherInfo));
	strcpy(lInfo->execPath, "disc0:/PSP_GAME/SYSDIR/BOOT.BIN");
	strcpy(lInfo->shellPath, "ms0:/MbShell/Artilery.elf");
	strcpy(lInfo->mInterface, "GRAPHPRX");
	strcpy(lInfo->mAudio, "AUDIOPRX");

	rebootPath = lInfo->shellPath;

	int thid;
	u32 func;

	printf("Patching kernel...");
	bdPatchKernel();

	printf("Starting backdoor thread...");
	// Relocate main thread below normal 0x89000000 to avoid data crunching
	func = (u32) main_thread;
	func |= 0x80000000;

	// Create a high priority thread
	thid = sceKernelCreateThread("main_thread", (void*) func, 0x20, 0x10000, 0, NULL);
	if(thid >= 0)
	{
		//Provide entry points args to the thread
		sceKernelStartThread(thid, argc, argv);
	}

	return 0;
}
