#include <pspmoduleinfo.h> 
#include <pspaudiolib.h>
#include <pspctrl.h>
#include <psppower.h>
#include <pspdebug.h>
#include "ASIncludes.h"

#include "Console.h"

PSP_MODULE_INFO("Artillery-Core", 0, 1, 0);
PSP_HEAP_SIZE_KB(13000);

ASConfig* ASCore::MainConfig;
ASConfigTheme* ASCore::MainConfigTheme;
ASDesktop* ASCore::MainDesktop;
ASUSBManager* ASCore::USB;
ASWiFiManager* ASCore::WiFi;
ASTaskbar* ASCore::Taskbar;
ASIconManager* ASCore::Icons;

ASCore::ASCore()
{
	MainConfig = new ASConfig(CFG_FILE);
	MainConfigTheme = new ASConfigTheme();
	
	USB = new ASUSBManager();
	WiFi = new ASWiFiManager();
	
	_controller = ASController::Instance();
	_cursor = new ASCursor();
	
	MainDesktop = new ASDesktop();
	
	Taskbar = new ASTaskbar();
	
	_windowManager = ASWindowManager::Instance();
	
	USB->ToggleUSB(true);
	
	_windowManager->CreateWindow(new Console(), "Console", 1, 0, 1);
	
	ASImage* test1 = new ASImage(900, 900);
	ASImage* test2 = new ASImage(400, 400);
	
	delete(test1);
	delete(test2);
}

/* Main core function */
void ASCore::Process()
{
	// Read in current control info
	sceCtrlReadBufferPositive(&_pad, 1);
	
	MainDesktop->Render();
	_windowManager->Render();
	Taskbar->Render();
	
	_controller->Handle(&_pad);
	_cursor->Handle(&_pad);
	
	_windowManager->Handle();
	Taskbar->Handle();
	MainDesktop->Handle();
	ProcessInput();
	
	// Update screen
	ASGraphics::Instance()->WaitVblankStart();
	ASGraphics::Instance()->FlipScreen();
}

void ASCore::ProcessInput()
{

}
