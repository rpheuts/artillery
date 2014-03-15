/*
# PSP Framework 				
# ------------------------------------------------------------------
# Author: R.P.Heuts
# Date: 23-05-2006
# Version: 0.1
# 
# Class: Controller
# Desciption:
# Provides access to the buttons and the ananlog stick of the PSP.
*/

#ifndef ASCONTROLLER_H
#define ASCONTROLLER_H
#include <pspctrl.h>

class ASController{
	public:
		static ASController* Instance();
		void Handle(SceCtrlData* pad);
		
	private:
		ASController();
		
	public:
		int LastInput;
		int PreviousInput;
		int LastInputRemote;
		int PreviousInputRemote;
		int IsLastInputPressedRemote;
		int IsLastInputPressed;
		int AnalogX;
		int AnalogY;
		int PreviousAnalogX;
		int PreviousAnalogY;
		int Handled;
		int RemotePluggedIn;
		
	private:
		static ASController* _instance;
};

#endif
