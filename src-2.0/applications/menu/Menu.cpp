#include "Menu.h"
#define MENU_WIDTH 200

Menu::Menu() : WindowApplication("Menu", MENU_WIDTH, 272)
{
	// Load the background
	_menu = Graphics::Instance()->Load(Core::MainConfigTheme->GetPathValue("c_menubackground"));
	_popup = Graphics::Instance()->Load(Core::MainConfigTheme->GetPathValue("c_menupopup"));
	this->Register();
			
	// Fill the button list, could be read from a config file?
	_buttons.push_back(new MenuButton("Run UMD", "Runs the current UMD", Core::MainConfigTheme->GetPathValue("c_menubuttonumd")));
	_buttons.push_back(new MenuButton("Homebrew", "Run homebrew", Core::MainConfigTheme->GetPathValue("c_menubuttonhomebrew")));
	_buttons.push_back(new MenuButton("Viewer", "Slideshow image viewer", Core::MainConfigTheme->GetPathValue("c_menubuttonviewer")));
	_buttons.push_back(new MenuButton("Music", "MP3 Player", Core::MainConfigTheme->GetPathValue("c_menubuttonmusic")));
	_buttons.push_back(new MenuButton("Reader", "Text file viewer", Core::MainConfigTheme->GetPathValue("c_menubuttonreader")));
	_buttons.push_back(new MenuButton("Settings", "Change shell settings", Core::MainConfigTheme->GetPathValue("c_menubuttonsettings")));
	_buttons.push_back(new MenuButton("Help", "Short introduction", Core::MainConfigTheme->GetPathValue("c_menubuttonhelp")));
	
	this->Settings->Transparent = 1;
	this->Settings->Borders = 0;
	this->Position(-MENU_WIDTH,0);
	_sliding = true;
	
	_activePopup = 0;
	_oldPopup = 0;
	
	UpdateButtons();
	_update = true;
}

Menu::~Menu()
{
	delete (_menu);
	delete (_popup);
	delete (_activePopup);
	delete (_oldPopup);
}

void Menu::HandleController(int mouseX, int mouseY)
{
	if (Controller::Instance()->IsPressed(PSP_CTRL_CROSS) && !Controller::Instance()->WasPressed(PSP_CTRL_CROSS))
	{
		// Run the currently selected application
		WindowApplication* window = new WindowApplication(_current->Name, 400, 240);
		window->Screen->Clear(0xFF992244);
		ArtilleryWindowManager::Instance()->RegisterArtilleryWindow(window);
	}
	if (Controller::Instance()->IsPressed(PSP_CTRL_DOWN) && !Controller::Instance()->WasPressed(PSP_CTRL_DOWN))
	{
		MenuButton* button = _buttons.front();
		_buttons.pop_front();
		_buttons.push_back(button);
		UpdateButtons();
	}
	else if (Controller::Instance()->IsPressed(PSP_CTRL_UP) && !Controller::Instance()->WasPressed(PSP_CTRL_UP))
	{
		MenuButton* button = _buttons.back();
		_buttons.pop_back();
		_buttons.push_front(button);
		UpdateButtons();
	}
}

void Menu::UpdateButtons()
{
	// Update the fading of buttons
	FadeButtons();
		
	// Spawn a new popup with information and remove the old one if there is one
	if (_oldPopup)
		delete (_oldPopup);
	if (_activePopup)
		_oldPopup = _activePopup;
	_activePopup = new MenuPopup(_popup, _current->Name, _current->Description, this->Width - 20, 320, this->Width - 20, 
				     100, this->Width - 20, -80);
	_update = true;
	this->Focus();
}

void Menu::FadeButtons()
{
	int fadeAmount = 25, count = 0;
	
	list<MenuButton*>::iterator buttonIt = _buttons.begin();
	list<MenuButton*>::iterator finalbutton = _buttons.end();
	while (buttonIt != finalbutton)
	{
		MenuButton* button = (MenuButton*)*buttonIt;
		button->ButtonDisplay->ApplyEffect(ImageAlpha, fadeAmount);
		
		if (count++ < 3)
			fadeAmount += 75;
		else if (count > 3 && count < 8)
			fadeAmount -= 75;
		else if (count > 6)
			break;
	
		if (count == 4)
			_current = button;
		
		buttonIt++;
	}
}

void Menu::UpdatePopups()
{
	if (_activePopup)
		_activePopup->UpdateActive();
	if (_oldPopup)
		_oldPopup->UpdateRemove();
}

void Menu::DestroyHandler()
{
	
}

void Menu::Render()
{
	if (_update)
	{
		// Render background
		_menu->BlitToImage(0,0, Screen);
		
		// Render buttons
		int height = 36;
		list<MenuButton*>::iterator buttonIt = _buttons.begin();
		list<MenuButton*>::iterator finalbutton = _buttons.end();
		while (buttonIt != finalbutton)
		{
			MenuButton* button = (MenuButton*)*buttonIt;
			button->ButtonDisplay->AlphaBlitToImage(16, height+=19, Screen);
			buttonIt++;
		}
		_update = false;
	}
	
	if (_sliding )
	{
		this->Position(this->X + 4,0);
		if (this->X == 0)
			_sliding = false;
	}
	else
		UpdatePopups();
	
}

void Menu::RenderFullscreen()
{
	
}
