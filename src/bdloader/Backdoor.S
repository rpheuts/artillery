	.set noreorder

#include "pspstub.s"

	STUB_START "Backdoor",0x00090000,0x00020005
	STUB_FUNC  0xEAC1837A,bdLaunchELF
	STUB_FUNC  0x5E013E67,bdLaunchUMD
	STUB_END
