#include <psptypes.h>
#include <pspusb.h>
#include <pspusbstor.h>

#include "artillery.h"

void USBManager::ToggleUSB()
{
	ToggleUSB(0);
}

void USBManager::ToggleUSB(int silent)
{
	int state = sceUsbGetState();
	if (state & PSP_USB_ACTIVATED)
	{
		sceUsbDeactivate(0x1c8);
		if (!silent)
			LCD::PrintMessage("USB Disabled");
	}
	else
	{
		sceUsbActivate(0x1c8);
		if (!silent)
			LCD::PrintMessage("USB Enabled");
	}
}

bool USBManager::IsEnabled()
{
	int state = sceUsbGetState();
	if (state & PSP_USB_ACTIVATED)
		return true;
	else
		return false;
}
