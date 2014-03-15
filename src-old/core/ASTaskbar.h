#ifndef ASTASKBAR_H_
#define ASTASKBAR_H_

#include "ASIncludes.h"

class ASTaskbar{
	public:
		ASTaskbar();
		~ASTaskbar();
		void Render();
		void Handle();
		int GetHeight();
	
	private:
		ASGraphics* _graphics;
		ASImage* _taskbar;
		ASImage* _usb;
		ASImage* _usbDis;
		ASImage* _wifi;
		ASImage* _wifiDis;
		ASImage* _screen;

		int _posY;
		int _offsetx;
		int _offsety;
		int _startup;
};
#endif
