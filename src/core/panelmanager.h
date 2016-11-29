#ifndef PANELCONT_H_
#define PANELCONT_H_

#include "artillery.h"

enum PanelLocation {Top, Bottom, Left, Right};

class Panel{
	public:
		Panel();
		virtual ~Panel();
		virtual void Handle(int mouseX, int mouseY, int btn);
		virtual int Render();
		
	public:
		Image* Display;
		Panel* Next;
		Panel* Previous;
		
	protected:
		int _screenUpdate;
};

class PanelContainer{
	public:
		PanelContainer();
		~PanelContainer();
		void AddPanel(Panel* panel);
		void Handle(int mouseX, int mouseY, int btn);
		void Show();
		void Hide();
		void Render();
	
	public:
		PanelLocation Location;
		PanelContainer* Next;
		PanelContainer* Previous;
		Image* Background;
		int Width;
		int Height;
		int X;
		int Y;
		int CurrentX;
		int CurrentY;
		int Visible;
		int Animate;
		Window* WndHnd;
	
	private:
		Panel* _pcListStart;
		Panel* _pcListEnd;
};

class PanelManager{
	public:
		static void Render();
		static void Handle(int mouseX, int mouseY, int btn);
		static PanelContainer* AddContainer(PanelLocation location, int width, Image* background);
		static void Destroy();
	
	private:
		static PanelContainer* _listStart;
		static PanelContainer* _listEnd;
		
};
#endif
