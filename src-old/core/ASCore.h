/*
# PSP Framework 				
# ------------------------------------------------------------------
# Author: R.P.Heuts
# Date: 23-05-2006
# Version: 0.1
# 
# Class: Core
# Desciption:
# This is the core class where the main thread resides. This class
# should maintain the other modules and share processing time among
# them. It will also maintain debugging output and general IO.
*/

#ifndef ASCORE_H
#define ASCORE_H

#include <pspctrl.h>

#include "ASIncludes.h"

/** The Core class is the center of the application. It loads and destroys
	all objects needed to run the application. The main loop resides
	in this class.
 */

class ASCore {
	public:
		/** Constructing this object will load most other classes needed to memory
			too.
		 */
		ASCore();
		
		/** This is the main process loop, this function should be called repeatedly
			to insure every window is updated within the application.
		 */
   		void Process();
		
	private:
		/** Function to provide input handling not handled by any of the modules.
		 */
   		void ProcessInput();
		
	public:
		static ASConfig* MainConfig;
		static ASConfigTheme* MainConfigTheme;
		static ASDesktop* MainDesktop;
		static ASUSBManager* USB;
		static ASWiFiManager* WiFi;
		static ASTaskbar* Taskbar;
		static ASIconManager* Icons; // temp?
		
	private:
		SceCtrlData _pad;
		ASController* _controller;
		ASCursor* _cursor;
		ASWindowManager* _windowManager;
   		
};
#endif
