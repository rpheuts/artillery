#include "Includes.h"

Controller* Controller::_instance = 0;

Controller* Controller::Instance()
{
	if (_instance == 0)
		_instance = new Controller();
	return _instance;
}

void Controller::Tick()
{
	_buttonsPressed = _driver->GetCurrentPressedButtons();
	_buttonsPressedPreviously = _driver->GetPreviousPressedButtons();
	_driver->GetCurrentCursorInfo(&CursorX, &CursorY);
	_driver->GetPreviousCursorInfo(&PreviousCursorX, &PreviousCursorY);
	Handled = 0;
}

bool Controller::IsPressed(int button)
{
	if ((button & _buttonsPressed) == button)
		return true;
	else return false;
}

bool Controller::WasPressed(int button)
{
	if ((button & _buttonsPressedPreviously) == button)
		return true;
	else return false;
}

int Controller::GetButtonMask()
{
	return _buttonsPressed;
}
		
Controller::Controller() : Module (1, "ControllerModule", 1)
{
	_driver = ControllerDriver::Instance();
	_buttonsPressed = 0;
	_buttonsPressedPreviously = 0;
	CursorX = 0;
	CursorY = 0;
	PreviousCursorX = 0;
	PreviousCursorY = 0;
}

Controller::~Controller()
{
	
}
