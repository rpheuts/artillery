#ifndef USBDRIVER_H
#define USBDRIVER_H

#include "Driver.h"

class USBDriver : public Driver
{
	public:
		static USBDriver* Instance();
		int Enable();
		int Disable();
		bool Enabled();
	private:
		USBDriver();
		~USBDriver();
		
	private:
		static USBDriver* _instance;
};

#endif
