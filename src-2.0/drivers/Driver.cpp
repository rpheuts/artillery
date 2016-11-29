#include "Driver.h"

Driver::Driver(int id, string name, bool ticking) : ID(id), Name(name), Ticking(ticking)
{
	_status = DriverStopped;
}

Driver::~Driver()
{

}

void Driver::Tick()
{
	
}

DriverStatus Driver::GetStatus()
{
	return _status;
}
