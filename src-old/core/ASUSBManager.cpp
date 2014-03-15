#include <psptypes.h>
#include <pspusb.h>
#include <pspusbstor.h>

#include "ASIncludes.h"

void ASUSBManager::ToggleUSB()
{
	ToggleUSB(0);
}

void ASUSBManager::ToggleUSB(bool silent)
{
	int state = sceUsbGetState();
	if (state & PSP_USB_ACTIVATED)
	{
		sceUsbDeactivate(0x1c8);
		//if (!silent)
		//	LCD::PrintMessage("USB Disabled");
	}
	else
	{
		sceUsbActivate(0x1c8);
		//if (!silent)
			//LCD::PrintMessage("USB Enabled");
	}
}

bool ASUSBManager::Enabled()
{
	int state = sceUsbGetState();
	if (state & PSP_USB_ACTIVATED)
		return true;
	else
		return false;
}
