#ifndef MENUPOPUP_H
#define MENUPOPUP_H

#include "Includes.h"

class MenuPopup
{
	public:
		MenuPopup(Image* background, string name, string content, int x, int y, int targetX, int targetY, int removeX, int removeY);
		~MenuPopup();
		void UpdateActive();
		void UpdateRemove();
		
	protected:
		void Init();
		void Update(int x, int y, bool remove);
		
	public:
		WindowApplication* PopupWindow;
		string Name;
		string Content;
		int TargetX;
		int TargetY;
		int RemoveX;
		int RemoveY;
	
	private:
		int _speed;
};

#endif
