#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "artillery.h"

class Console{
	public:
		static void Init();
		static void Print(char * text);
		static void Print(const char * text);
		static void Print(string text);
		
	protected:
		static void HandleController(int analogX, int analogY, int btn);
		static void HandleDestroy();
		static void Render();
};
#endif
