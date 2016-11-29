#ifndef MODULE_H
#define MODULE_H

#include <string>
using namespace std;

enum ModuleStatus {ModuleStopped, ModuleRunning, ModuleError, ModuleCritical};

/** Base Module class
 */
class Module
{
	public:
		/** Initializes the base driver, set tick to true to enable
		updating of the driver every core tick*/
		Module(int id, string name, bool ticking);
		
		virtual ~Module();
		
		/** Called every core tick to update the driver if needed */
		virtual void Tick();
		
		/** Should return the status of the driver */
		virtual ModuleStatus GetStatus();
		
	public:
		int ID;
		string Name;
		bool Ticking;
		
	protected:
		ModuleStatus _status;
};

#endif
