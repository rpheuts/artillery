#ifndef MENU_H
#define MENU_H

#include "Includes.h"
#include "MenuButton.h"
#include "MenuPopup.h"

class Menu : public WindowApplication
{
	public:
		Menu();
		~Menu();
		void HandleController(int mouseX, int mouseY);
		void DestroyHandler();
		void Render();
		void RenderFullscreen();
		void FadeButtons();
		void UpdatePopups();
		void UpdateButtons();
		
	private:
		list<MenuButton*> _buttons;
		MenuButton* _current;
		Image* _menu;
		Image* _popup;
		bool _sliding;
		bool _update;
		MenuPopup* _activePopup;
		MenuPopup* _oldPopup;
};

#endif
