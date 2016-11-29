#ifndef MENUBUTTON_H
#define MENUBUTTON_H

#include "Includes.h"

class MenuButton
{
	public:
		MenuButton(string name, string description, string imagepath);
		~MenuButton();
	
	public:
		string Name;
		string Description;
		Image* ButtonImage;
		Image* ButtonDisplay;
			
};


#endif
