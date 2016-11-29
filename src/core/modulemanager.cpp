#include <psptypes.h>
#include <pspumd.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "artillery.h"

int ModuleManager::LoadStartModule(const char *path, const char* arg)
{
    u32 loadResult;
    u32 startResult;
    int status;

    loadResult = sceKernelLoadModule(path, 0, NULL);
    if (loadResult & 0x80000000)
		return -1;
    else
	 {
	 	if (arg)
	 	{
	 		char tmp[strlen(arg) + 1];
	 		strcpy(tmp, arg);
			startResult =sceKernelStartModule(loadResult, sizeof(tmp), tmp, &status, NULL);
		}
		else
			startResult =sceKernelStartModule(loadResult, 0, NULL, &status, NULL);
	 }
    if (loadResult != startResult)
		return -2;
    return 0;
}

void ModuleManager::LoadStartUMD()
{
	int res = sceUmdCheckMedium(0);
	if(res != 0)
	{
		LoadStartModule("ms0:/MbShell/modules/kernmod.prx", NULL);
		sceKernelExitDeleteThread(0);
	}
	else
		LCD::PrintMessage("No UMD found");
}

void ModuleManager::LoadStartMPH()
{
	int res = sceUmdCheckMedium(0);
	if(res != 0)
	{
		LoadStartModule("ms0:/MbShell/modules/kernmod.prx", "ms0:/MbShell/modules/mphload.pbp");
		sceKernelExitDeleteThread(0);
	}
	else
		LCD::PrintMessage("No UMD found");
}

void ModuleManager::Reboot()
{
	LoadStartELF("ms0:/MbShell/Artillery.elf");
}

void ModuleManager::LoadStartELF(const char * path)
{
	LoadStartModule("ms0:/MbShell/modules/kernmod.prx", path);
	sceKernelExitDeleteThread(0);
}
