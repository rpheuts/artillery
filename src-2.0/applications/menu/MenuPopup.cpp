#include "Menu.h"

MenuPopup::MenuPopup(Image* background, string name, string content, int x, int y, int targetX, int targetY, int removeX, int removeY)
{
	this->PopupWindow = new WindowApplication("Popup", background->Width, background->Height);
	this->PopupWindow->Register();
	this->PopupWindow->Position(x, y);
	this->TargetX = targetX;
	this->TargetY = targetY;
	this->RemoveX = removeX;
	this->RemoveY = removeY;
	this->Name = name;
	this->Content = content;
	background->BlitToImage(0, 0, PopupWindow->Screen);
	Init();
}

MenuPopup::~MenuPopup()
{
	PopupWindow->Destroy();
	delete (PopupWindow);
}

void MenuPopup::Init()
{
	this->PopupWindow->Settings->Transparent = 1;
	this->PopupWindow->Settings->Borders = 0;
	
	PopupWindow->Screen->Print(Name, 40, 5, 0xAAFFFFFF, 0, 1);
	PopupWindow->Screen->Print(Content, 40, 20, 0xAAFFFFFF, 0, 0);
	
	_speed = 5;
}

void MenuPopup::UpdateActive()
{
	Update(this->TargetX, this->TargetY, false);
}

void MenuPopup::UpdateRemove()
{
	Update(this->RemoveX, this->RemoveY, true);
}

void MenuPopup::Update(int x, int y, bool remove)
{
	if (this->PopupWindow->X != x)
	{
		if (this->PopupWindow->X < x)
			this->PopupWindow->Position(this->PopupWindow->X + _speed, this->PopupWindow->Y);
		else
			this->PopupWindow->Position(this->PopupWindow->X - _speed, this->PopupWindow->Y);
		
		if (remove)
			this->PopupWindow->Screen->ApplyEffect(ImageFade, -10);
	}
	
	if (this->PopupWindow->Y != y)
	{
		if (this->PopupWindow->Y < y)
			this->PopupWindow->Position(this->PopupWindow->X, this->PopupWindow->Y + _speed);
		else
			this->PopupWindow->Position(this->PopupWindow->X, this->PopupWindow->Y - _speed);
		
		if (remove)
			this->PopupWindow->Screen->ApplyEffect(ImageFade, -10);
	}
}
