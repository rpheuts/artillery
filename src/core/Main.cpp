#include <pspmoduleinfo.h> 
#include "Core.h"

PSP_MODULE_INFO("Artillery-Core", 0, 1, 0);
PSP_HEAP_SIZE_KB(20*1024);

int main(int args, char **argp)
{
	Core* core = new Core();
	core->CoreEntry();
}
