#ifndef CORE_H
#define CORE_H

#include "artillery.h"

class Core {
	public:
   		static bool Init();
		static void Destroy();
		static void Reload();
   		static void Process();
	
	public:
		static ConfigTheme* CfgTheme;
		static Config* Cfg;
};
#endif
