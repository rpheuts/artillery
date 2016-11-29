#include <pspctrl.h>

class Controller{
	public:
		static int LastInput;
		static int PreviousInput;
		static int LastInputRemote;
		static int PreviousInputRemote;
		static int IsLastInputPressedRemote;
		static int IsLastInputPressed;
		static int AnalogX;
		static int AnalogY;
		static int PreviousAnalogX;
		static int PreviousAnalogY;
		static int Handled;
		static int RemotePluggedIn;

	public:
		static void Handle(SceCtrlData* pad);
		static void Init();
};


