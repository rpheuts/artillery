#include "Menu.h"

MenuButton::MenuButton(string name, string description, string imagepath)
{
	ButtonImage = Graphics::Instance()->Load(imagepath);
	ButtonDisplay = new Image(ButtonImage->Width, ButtonImage->Height);
	ButtonImage->BlitToImage(0, 0, ButtonDisplay);
	Name = name;
	Description = description;
}

MenuButton::~MenuButton()
{
	delete (ButtonImage);
	delete (ButtonDisplay);
}
