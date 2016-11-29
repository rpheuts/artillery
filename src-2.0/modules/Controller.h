#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Includes.h"

class Controller : public Module
{
	public:
		static Controller* Instance();
		void Tick();
		bool IsPressed(int button);
		bool WasPressed(int button);
		int GetButtonMask();
		
		
	protected:
		Controller();
		~Controller();
		
	public:
		int CursorX;
		int CursorY;
		int PreviousCursorX;
		int PreviousCursorY;
		int Handled;
		
	private:
		static Controller* _instance;
		ControllerDriver* _driver;
		int _buttonsPressed;
		int _buttonsPressedPreviously;
};

#endif
