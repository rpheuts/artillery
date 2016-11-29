#include "core.h"
#include <pspkernel.h>

int main(int args, char **argp)
{
	Core::Init();
	// Run until die for now
	while (1)
	{
		Core::Process();
	}
}
