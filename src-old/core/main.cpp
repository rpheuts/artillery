/*
# PSP Framework 				
# ------------------------------------------------------------------
# Author: R.P.Heuts
# Date: 23-05-2006
# Version: 0.1
# 
# Class: Main entry point
# Desciption:
# This is the main entry point for the module (PRX) the main class
# core is instantiated here and the main process function is called
# until the application terminates.
*/

#include "ASCore.h"
#include <pspkernel.h>

int main(int args, char **argp)
{
	ASCore* core = new ASCore();
	// Run until die for now
	while (1)
	{
		core->Process();
	}
}
