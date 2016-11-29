#ifndef CONTROLLERDRIVER_H
#define CONTROLLERDRIVER_H

#include "Driver.h"

class ControllerDriver : public Driver
{
	public:
		static ControllerDriver* Instance();
		void Tick();
		int GetCurrentPressedButtons();
		int GetPreviousPressedButtons();
		int GetCurrentCursorInfo(int* cursorX, int* cursorY);
		int GetPreviousCursorInfo(int* cursorX, int* cursorY);
		
	private:
		ControllerDriver();
		~ControllerDriver();
		
	private:
		static ControllerDriver* _instance;
		int _lastInput;
		int _previousInput;
		int _isLastInputPressed;
		int _analogX;
		int _analogY;
		int _previousAnalogX;
		int _previousAnalogY;
};

#endif
