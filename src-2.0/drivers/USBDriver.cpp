#include "USBDriver.h"
#include <psptypes.h>
#include <pspusb.h>
#include <pspusbstor.h>

USBDriver* USBDriver::_instance = 0;

USBDriver* USBDriver::Instance()
{
	if (_instance == 0)
		_instance = new USBDriver();
	return _instance;
}

USBDriver::USBDriver() : Driver(2, "USB", false)
{
	Enable();
	_status = DriverRunning;
}

USBDriver::~USBDriver()
{
	
}

int USBDriver::Enable()
{
	int state = sceUsbGetState();
	if (!(state & PSP_USB_ACTIVATED))
	{
		sceUsbActivate(0x1c8);
		return 0;
	}
	return -1;
}

int USBDriver::Disable()
{
	int state = sceUsbGetState();
	if (state & PSP_USB_ACTIVATED)
	{
		sceUsbDeactivate(0x1c8);
		return 0;
	}
	return -1;
}

bool USBDriver::Enabled()
{
	int state = sceUsbGetState();
	if (state & PSP_USB_ACTIVATED)
		return true;
	return false;
}
