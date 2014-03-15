#ifndef CORE_H
#define CORE_H

#include "Includes.h"

class Core
{
	public:
		Core();
		~Core();
		void CoreEntry();
	
	private:
		void InitDrivers();
		void InitConfig();
		void InitModules();
		void CoreTick();
		void DriverErrorHandler(Driver* driver);
		void ModuleErrorHandler(Module* module);
		int DebugFreeMem();
	
	public:
		static Config* MainConfig;
		static ConfigTheme* MainConfigTheme;
		
	private:
		list<Driver*> _drivers;
		list<Module*> _modules;
};

#endif
