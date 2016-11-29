#include <malloc.h>
#include <pspctrl.h>
#include <psphprm.h>
#include "artillery.h"

int Controller::LastInput;
int Controller::LastInputRemote;
int Controller::AnalogX;
int Controller::AnalogY;
int Controller::IsLastInputPressed;
int Controller::IsLastInputPressedRemote;
int Controller::PreviousInput;
int Controller::PreviousInputRemote;
int Controller::PreviousAnalogX;
int Controller::PreviousAnalogY;
int Controller::Handled;
int Controller::RemotePluggedIn;

void Controller::Init()
{
	
}

void Controller::Handle(SceCtrlData* pad)
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
	
	if (pad->Buttons != 0){
	
			if (pad->Buttons & PSP_CTRL_LTRIGGER && pad->Buttons & PSP_CTRL_RTRIGGER && pad->Buttons & PSP_CTRL_START)
				ModuleManager::Reboot();
			
			if (pad->Buttons & PSP_CTRL_SQUARE){
				LastInput = PSP_CTRL_SQUARE;
				return;
			}
			if (pad->Buttons & PSP_CTRL_TRIANGLE){
				LastInput = PSP_CTRL_TRIANGLE;
				return;
			} 
			if (pad->Buttons & PSP_CTRL_CIRCLE){
				LastInput = PSP_CTRL_CIRCLE;
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
				if (!IsLastInputPressed)
					LCD::SwitchMode();
				return;
			}
			if (pad->Buttons & PSP_CTRL_SELECT){
				Image* tmp = Graphics::GrabScreenBuffer();
				Graphics::SavePNG("ms0:/MbShell/screenshot.png", tmp->data, tmp->imageWidth, tmp->imageHeight, tmp->textureWidth, 0);
				Graphics::FreeImage(tmp);
				LastInput = PSP_CTRL_SELECT;
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
