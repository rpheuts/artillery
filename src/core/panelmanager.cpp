#include "artillery.h"

PanelContainer* PanelManager::_listStart;
PanelContainer* PanelManager::_listEnd;

Panel::Panel()
{
	Panel::_screenUpdate = 1;
}

Panel::~Panel()
{
	// just here to bypass a strange error in the sdk... :/
}

void Panel::Handle(int mouseX, int mouseY, int btn)
{
	// just here to bypass a strange error in the sdk... :/
}

int Panel::Render()
{
	return 0;
	// just here to bypass a strange error in the sdk... :/
}

PanelContainer::PanelContainer()
{
	PanelContainer::_pcListStart = NULL;
}

PanelContainer::~PanelContainer()
{
	Panel* panel = PanelContainer::_pcListStart;
	Panel* rem = NULL;
	while (panel != NULL)
	{
		Graphics::FreeImage(panel->Display);
		rem = panel;
		panel = panel->Next;
		delete (rem);
	}
}

void PanelContainer::Handle(int mouseX, int mouseY, int btn)
{
	if (this->Visible == 1)
	{
		Panel* panel = PanelContainer::_pcListStart;
		while (panel != NULL)
		{
			panel->Handle(mouseX, mouseY, btn);
			panel = panel->Next;
		}
	}
}

void PanelContainer::Render()
{
	Panel* panel = PanelContainer::_pcListStart;
	if (Animate)
	{
		if (Visible)
		{
			if (CurrentX > X)
			{
				CurrentX -= 4;
				WindowManager::WindowPosition(PanelContainer::WndHnd, CurrentX, Y);
			}
			else
				Animate = 0;
		}
		else
		{
			if (CurrentX < SCREEN_WIDTH-17)
			{
				CurrentX += 4;
				WindowManager::WindowPosition(PanelContainer::WndHnd, CurrentX, Y);
			}
			else
				Animate = 0;
		}
	}
	while (panel != NULL)
	{
		if(panel->Render())
		{
			Graphics::BlitImageToImage(0,0,Background->imageWidth, Background->imageHeight, Background,0,0,PanelContainer::WndHnd->Screen);
			Graphics::BlitAlphaImageToImage(0,0,panel->Display->imageWidth, panel->Display->imageHeight, panel->Display,20,0,PanelContainer::WndHnd->Screen);
		}
		panel = panel->Next;
	}
}

void PanelContainer::AddPanel(Panel* panel)
{
	panel->Display = Graphics::CreateImage(PanelContainer::WndHnd->Screen->imageWidth, PanelContainer::WndHnd->Screen->imageHeight);
	if (PanelContainer::_pcListStart == NULL)
	{
		PanelContainer::_pcListStart = panel;
		PanelContainer::_pcListStart->Next = NULL;
		PanelContainer::_pcListStart->Previous = NULL;
		PanelContainer::_pcListEnd = PanelContainer::_pcListStart;
	}
	Handle(0,0,0);
}

void PanelContainer::Show()
{
	WindowManager::WindowPosition(PanelContainer::WndHnd, X, Y);
	Animate = 1;
	Visible = 1;
}

void PanelContainer::Hide()
{
	WindowManager::WindowPosition(PanelContainer::WndHnd, SCREEN_WIDTH-17, 0);
	Animate = 1;
	Visible = 0;
}

void PanelManager::Destroy()
{

}

void PanelManager::Handle(int mouseX, int mouseY, int btn)
{
	PanelContainer* container = _listStart;
	while (container != NULL)
	{
		container->Handle(mouseX, mouseY, btn);
		container = container->Next;
	}
}

void PanelManager::Render()
{
	PanelContainer* container = _listStart;
	while (container != NULL)
	{
		container->Render();
		container = container->Next;
	}
}

PanelContainer* PanelManager::AddContainer(PanelLocation location, int managerwidth, Image* background)
{
	int x = 0,y = 0,height = 0,width = 0;
	
	switch (location)
	{
		case Top:
			x = 0;
			y = 0;
			height = managerwidth;
			width = SCREEN_WIDTH;
		break;
		
		case Bottom:
			x = 0;
			y = SCREEN_HEIGHT - managerwidth;
			height = managerwidth;
			width = SCREEN_WIDTH;
		break;
		
		case Right:
			x = SCREEN_WIDTH - managerwidth;
			y = 0;
			height = SCREEN_HEIGHT;
			width = managerwidth;
		break;
		
		case Left:
			x = 0;
			y = 0;
			height = SCREEN_HEIGHT;
			width = managerwidth;
		break;
	}
	
	PanelContainer* container = new PanelContainer();
	container->WndHnd = WindowManager::CreateWindow(width, height, Handle, Render, Destroy, "Panel", 1, 0);
	container->WndHnd->Screen = Graphics::CreateImage(width, height);
	container->Location = location;
	container->Width = width;
	container->Height = height;
	container->X = x;
	container->Y = y;
	container->CurrentX = x;
	container->CurrentY = y;
	container->Visible = 1;
	container->Background = background;
	WindowManager::WindowPosition(container->WndHnd, x, y);
	
	if (PanelManager::_listStart == NULL)
	{
		_listStart = container;
		_listStart->Next = NULL;
		_listStart->Previous = NULL;
		_listEnd = _listStart;
	}
	else
	{
		// Window gets focus, so add to the beginning of the window list
		container->Next =  _listStart;
		container->Next->Previous = container;
		container->Previous = NULL;
		_listStart = container;
	}
	return container;
	return NULL;
}
