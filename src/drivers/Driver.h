#ifndef DRIVER_H
#define DRIVER_H

#include <string>
using namespace std;

enum DriverStatus {DriverStopped, DriverRunning, DriverError, DriverCritical};

/** Base Driver class
*/
class Driver
{
	public:
		/** Initializes the base driver, set tick to true to enable
		updating of the driver every core tick*/
		Driver(int id, string name, bool ticking);
		
		virtual ~Driver();
		
		/** Called every core tick to update the driver if needed */
		virtual void Tick();
		
		/** Should return the status of the driver */
		virtual DriverStatus GetStatus();
		
	public:
		int ID;
		string Name;
		bool Ticking;
		
	protected:
		DriverStatus _status;
};

#endif
