#include <malloc.h>
#include <pspctrl.h>
#include <psphprm.h>
#include "ASIncludes.h"

ASController* ASController::_instance = 0;

ASController* ASController::Instance()
{
	if (_instance == 0)
		_instance = new ASController();
	return _instance;
}

ASController::ASController()
{
	//Core::Dbg->Log(Init, "Controller Library loaded...");
}

void ASController::Handle(SceCtrlData* pad)
{
	PreviousInput = LastInput;
	Handled = 0;
	u32 remoteButtons;
	sceHprmPeekCurrentKey(&remoteButtons);
	
	if (sceHprmIsRemoteExist() == 1)
	{
		PreviousInputRemote = LastInputRemote;
		if (remoteButtons & PSP_HPRM_PLAYPAUSE)
      {
      	LastInputRemote = PSP_HPRM_PLAYPAUSE;
			if (PreviousInputRemote == PSP_HPRM_PLAYPAUSE)
				IsLastInputPressedRemote = 1;
      }
      else if (remoteButtons & PSP_HPRM_BACK)
      {
         LastInputRemote = PSP_HPRM_BACK;
			if (PreviousInputRemote == PSP_HPRM_BACK)
				IsLastInputPressedRemote = 1;
      }
      else if (remoteButtons & PSP_HPRM_FORWARD)
      {
      	LastInputRemote = PSP_HPRM_FORWARD;
			if (PreviousInputRemote == PSP_HPRM_FORWARD)
				IsLastInputPressedRemote = 1; 
      }
    	if (PreviousInputRemote == LastInputRemote)
    		LastInputRemote = -1;
	}
	
	if (pad->Buttons != 0)
	{
			//if (pad->Buttons & PSP_CTRL_LTRIGGER && pad->Buttons & PSP_CTRL_RTRIGGER && pad->Buttons & PSP_CTRL_START)
				//Core::ModMan->Reboot();
			if (pad->Buttons & PSP_CTRL_SQUARE){
				LastInput = PSP_CTRL_SQUARE;
				if (PreviousInput == PSP_CTRL_SQUARE)
					IsLastInputPressed = 1;
				return;
			}
			if (pad->Buttons & PSP_CTRL_TRIANGLE){
				LastInput = PSP_CTRL_TRIANGLE;
				if (PreviousInput == PSP_CTRL_TRIANGLE)
					IsLastInputPressed = 1;
				return;
			} 
			if (pad->Buttons & PSP_CTRL_CIRCLE){
				LastInput = PSP_CTRL_CIRCLE;
				if (PreviousInput == PSP_CTRL_CIRCLE)
					IsLastInputPressed = 1;
				return;
			} 
			if (pad->Buttons & PSP_CTRL_CROSS){
				LastInput = PSP_CTRL_CROSS;
				if (PreviousInput == PSP_CTRL_CROSS)
					IsLastInputPressed = 1;
				return;
			} 

			if (pad->Buttons & PSP_CTRL_UP){
				LastInput = PSP_CTRL_UP;
				if (PreviousInput == PSP_CTRL_UP)
					IsLastInputPressed = 1;
				return;
			} 
			if (pad->Buttons & PSP_CTRL_DOWN){
				LastInput = PSP_CTRL_DOWN;
				if (PreviousInput == PSP_CTRL_DOWN)
					IsLastInputPressed = 1;
				return;
			} 
			if (pad->Buttons & PSP_CTRL_LEFT){
				LastInput = PSP_CTRL_LEFT;
				if (PreviousInput == PSP_CTRL_LEFT)
					IsLastInputPressed = 1;
				return;
			} 
			if (pad->Buttons & PSP_CTRL_RIGHT){
				LastInput = PSP_CTRL_RIGHT;
				if (PreviousInput == PSP_CTRL_RIGHT)
					IsLastInputPressed = 1;
				return;
			}      

			if (pad->Buttons & PSP_CTRL_START){
				LastInput = PSP_CTRL_START;
				if (PreviousInput == PSP_CTRL_START)
					IsLastInputPressed = 1;
				return;
			}
			if (pad->Buttons & PSP_CTRL_SELECT){
				LastInput = PSP_CTRL_SELECT;
				if (PreviousInput == PSP_CTRL_SELECT)
					IsLastInputPressed = 1;
				return;
			}
			if (pad->Buttons & PSP_CTRL_LTRIGGER){
				LastInput = PSP_CTRL_LTRIGGER;
				if (PreviousInput == PSP_CTRL_LTRIGGER)
					IsLastInputPressed = 1;
				return;
			}
			if (pad->Buttons & PSP_CTRL_RTRIGGER){
				LastInput = PSP_CTRL_RTRIGGER;
				if (PreviousInput == PSP_CTRL_RTRIGGER)
					IsLastInputPressed = 1;
				return;
			}
		}
		LastInput =-1;
		if (IsLastInputPressed)
			IsLastInputPressed = 0;
}
