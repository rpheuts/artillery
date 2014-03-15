#include "Includes.h"
#include <malloc.h>

Config* Core::MainConfig;
ConfigTheme* Core::MainConfigTheme;

Core::Core()
{
	InitDrivers();
	InitConfig();
	InitModules();
	ScreenDriver::Instance()->SetScreenMode(Graphical);
}

Core::~Core()
{
	
}

void Core::CoreEntry()
{
	Menu* menu = new Menu();
	while (1)
	{
		CoreTick();
	}
}

void Core::InitDrivers()
{
	// Initialize the Screen Driver
	_drivers.push_back(ScreenDriver::Instance());
	
	// Initialize the USB Driver
	_drivers.push_back(USBDriver::Instance());
	
	// Initialize the Controller Driver
	_drivers.push_back(ControllerDriver::Instance());
}

void Core::InitModules()
{
	// Initialize the Controller Module
	_modules.push_back(Controller::Instance());
	
	// Initialize the Desktop Module
	_modules.push_back(Desktop::Instance());
	
	// Initialize the WindowManager Module
	_modules.push_back(ArtilleryWindowManager::Instance());
	
}

void Core::InitConfig()
{
	MainConfig = new Config(new File(CFG_FILE));
	MainConfigTheme = new ConfigTheme();
}
		
void Core::CoreTick()
{
	// Give the drivers some processing time
	list<Driver*>::iterator driverIt = _drivers.begin();
	list<Driver*>::iterator finaldriver = _drivers.end();
	while (driverIt != finaldriver)
	{
		Driver* driver = (Driver*)*driverIt;
		// Check if the driver has malfunctioned
		if (driver->GetStatus() == DriverCritical)
			DriverErrorHandler(driver);
		
		// Update the driver if it requires so
		if (driver->Ticking)
			driver->Tick();
		
		++driverIt;
	}
	
	// Give the Modules some processing time
	list<Module*>::iterator moduleIt = _modules.begin();
	list<Module*>::iterator finalmodule = _modules.end();
	while (moduleIt != finalmodule)
	{
		Module* module = (Module*)*moduleIt;
		// Check if the driver has malfunctioned
		if (module->GetStatus() == ModuleCritical)
			ModuleErrorHandler(module);
		
		// Update the driver if it requires so
		if (module->Ticking)
			module->Tick();
		
		++moduleIt;
	}
}

void Core::DriverErrorHandler(Driver* driver)
{
	
}

void Core::ModuleErrorHandler(Module* module)
{
	
}

int Core::DebugFreeMem()
{
	struct mallinfo mi;
	mi = mallinfo();
	return ((20*1024*1024) - mi.arena + mi.fordblks); 
}
