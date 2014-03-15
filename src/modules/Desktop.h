#ifndef DESKTOP_H
#define DESKTOP_H

#include "Includes.h"

class Desktop : public Module
{
	public:
		static Desktop* Instance();
		void Tick();
		
	protected:
		Desktop();
		~Desktop();
		
	protected:
		Image* _wallpaper;
		
	private:
		static Desktop* _instance;
};

#endif
