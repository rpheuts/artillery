#include "Module.h"

Module::Module(int id, string name, bool ticking) : ID(id), Name(name), Ticking(ticking)
{
	_status = ModuleStopped;
}

Module::~Module()
{

}

void Module::Tick()
{
	
}

ModuleStatus Module::GetStatus()
{
	return _status;
}
