#include "ControllerDriver.h"
#include <psptypes.h>
#include <pspctrl.h>

ControllerDriver* ControllerDriver::_instance = 0;

ControllerDriver* ControllerDriver::Instance()
{
	if (_instance == 0)
		_instance = new ControllerDriver();
	return _instance;
}

ControllerDriver::ControllerDriver() : Driver(3, "Controller", true)
{
	_lastInput = 0;
	_previousInput = 0;
	_isLastInputPressed = 0;
	_analogX = 0;
	_analogY = 0;
	_previousAnalogX = 0;
	_previousAnalogY = 0;
	_status = DriverRunning;
}

ControllerDriver::~ControllerDriver()
{
	
}

void ControllerDriver::Tick()
{
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	
	_previousInput = _lastInput;
	_lastInput = pad.Buttons;
}

int ControllerDriver::GetCurrentPressedButtons()
{
	return _lastInput;
}

int ControllerDriver::GetPreviousPressedButtons()
{
	return _previousInput;
}

int ControllerDriver::GetCurrentCursorInfo(int* cursorX, int* cursorY)
{
	*cursorX = 0;
	*cursorY = 0;
	return 0;
}

int ControllerDriver::GetPreviousCursorInfo(int* cursorX, int* cursorY)
{
	*cursorX = 0;
	*cursorY = 0;
	return 0;
}
