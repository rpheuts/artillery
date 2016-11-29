#include "Includes.h"

#include <string.h>
#include <pspctrl.h>

WindowManager::WindowManager() : Module(1, "WindowManagerModule", 1)
{
	_createdLastX = 5;
	_createdLastY = 2;
	_titlebarTextOffsetX = 0;
	_titlebarTextOffsetY = 0;
	_graphics = Graphics::Instance();
	_controller = Controller::Instance();
	
	ConfigTheme* mainCfg = Core::MainConfigTheme;
	_shadowHorizontal = _graphics->Load(mainCfg->GetPathValue("c_wndborderhor"));
	_shadowVerticalRight = _graphics->Load(mainCfg->GetPathValue("c_wndborderverright"));
	_shadowVerticalLeft = _graphics->Load(mainCfg->GetPathValue("c_wndborderverleft"));
	_shadowCornerRight = _graphics->Load(mainCfg->GetPathValue("c_wndbordercorright"));
	_shadowCornerLeft = _graphics->Load(mainCfg->GetPathValue("c_wndbordercorleft"));
	_buttonClose = _graphics->Load(mainCfg->GetPathValue("c_wndtopbarbtn"));
	_topbarLeft = _graphics->Load(mainCfg->GetPathValue("c_wndtopbarleft"));
	_topbarRight = _graphics->Load(mainCfg->GetPathValue("c_wndtopbarright"));
	_topbarCenter = _graphics->Load(mainCfg->GetPathValue("c_wndtopbarcenter"));
	_topbarScaleLeft = _graphics->Load(mainCfg->GetPathValue("c_wndtopbarscaleleft"));                                        
	_topbarScaleRight = _graphics->Load(mainCfg->GetPathValue("c_wndtopbarscaleright"));
	mainCfg->GetIntValue("titlebartextoffsetx", _titlebarTextOffsetX);
	mainCfg->GetIntValue("titlebartextoffsety", _titlebarTextOffsetY);
	mainCfg = 0;
	
	_status = ModuleRunning;
}

WindowManager::~WindowManager()
{
	delete (_shadowHorizontal);
	delete (_shadowVerticalRight);
	delete (_shadowVerticalLeft);
	delete (_shadowCornerRight);
	delete (_shadowCornerLeft);
	delete (_buttonClose);
	delete (_topbarLeft);
	delete (_topbarRight);
	delete (_topbarCenter);
	delete (_topbarScaleLeft);
	delete (_topbarScaleRight);
	
	_windows.clear();
}

void WindowManager::DestroyWindow(WindowApplication* window)
{
	window->DestroyHandler();
	if (window->Screen)
		delete (window->Screen);
	delete (window->Topbar);
	
	_windows.remove(window);
}

void WindowManager::RegisterWindow(WindowApplication* window)
{
	if (window->X == -1)
	{
		window->X = _createdLastX;
		_createdLastX += 20;
	}
	if (window->Y == -1)
	{
		window->Y = _createdLastY;
		_createdLastY += 10;
	}
	window->Topbar = InitTopbar(window->Width, window->Name);
	
	window->ContentX = window->X + _shadowVerticalLeft->Width;
	window->ContentY = window->Y + window->Topbar->Height;
	window->Height += window->Topbar->Height;
	
	_windows.push_front(window);
}

Image* WindowManager::InitTopbar(int width, string text)
{
	int index = 0;
	Image* topbar =  new Image(width, _topbarLeft->Height);
	Graphics* graphics = Graphics::Instance();
	
	// Blit left part first
	_topbarLeft->BlitToImage(_topbarLeft->Width, _topbarLeft->Height, 0, 0, topbar);
	index += _topbarLeft->Width;
	
	// Blit scaling part for text
	Image* tmp = graphics->ResizeImage((text.size()*6)+_titlebarTextOffsetX, _topbarScaleLeft->Height, _topbarScaleLeft);
	tmp->BlitToImage(tmp->Width, tmp->Height, index, 0, topbar);
	index += tmp->Width;
	delete (tmp);
	
	// Blit center part
	_topbarCenter->BlitToImage(_topbarCenter->Width, _topbarCenter->Height, index, 0, topbar);
	index += _topbarCenter->Width;
	
	// Blit scaling right part
	tmp = graphics->ResizeImage(width - index - _topbarRight->Width, _topbarScaleRight->Height, _topbarScaleRight);
	tmp->BlitToImage(tmp->Width, tmp->Height, index, 0, topbar);
	index += tmp->Width;
	delete (tmp);
	
	// Blit last part
	_topbarRight->BlitToImage(_topbarRight->Width, _topbarRight->Height, index, 0, topbar);
	
	// Blit close button
	_buttonClose->AlphaBlitToImage(_buttonClose->Width, _buttonClose->Height, width - _buttonClose->Width - 3, 0, topbar);
	
	// Print text
	topbar->Print(text, _titlebarTextOffsetX, _titlebarTextOffsetY, 0xFFFFFFFF, 0, 0);
	return topbar;
}

void WindowManager::Focus(WindowApplication* window)
{
	_windows.remove(window);
	_windows.push_front(window);
	_isActive = 1;
}

void WindowManager::WindowPosition(WindowApplication* window, int x, int y)
{
	window->X = x;
	window->Y = y;
	window->ContentX = x;
	window->ContentY = y + window->Topbar->Height;
}

void WindowManager::HandleController()
{
	if (_windows.size() > 0)
	{
		WindowApplication* window = _windows.front();
		
		if ((Interface::IsWithinWindowContent(_controller->CursorX, _controller->CursorY, window) || _isActive) && window->Settings->Enabled == 1)
		{
			if (!window->Settings->Fullscreen)
				window->HandleController(_controller->CursorX - window->ContentX, _controller->CursorY - window->ContentY);
			else
				window->HandleController(_controller->CursorX, _controller->CursorY);
			_controller->Handled = 1;
		}
		else
			_isActive = 0;
	}
}

void WindowManager::Tick()
{
	if (!_controller->Handled)
		HandleController();
	Render();
}

void WindowManager::Render()
{
	list<WindowApplication*>::reverse_iterator windowIt = _windows.rbegin();
	list<WindowApplication*>::reverse_iterator finalWindow = _windows.rend();
	WindowApplication* ontop = 0;
	while (windowIt != finalWindow)
	{
		WindowApplication* window = (WindowApplication*)*windowIt;
		if (window->Settings->Enabled == 1)
		{
			window->Render();
			if (window->Settings->Fullscreen)
			{
				window->RenderFullscreen();
				FullScreenEnabled = 1;
				break;
			}
			
			if (window->Settings->OnTop == 1)
				ontop = window;
			else
				RenderWindow(window);
		}
		windowIt++;
	}
	if (ontop)
		RenderWindow(ontop);
	ontop = 0;
}

void WindowManager::RenderWindow(WindowApplication* t)
{
	int width, height, offsetx, offsety, posx, posy;
	
	if (!(t->ContentY >= 272) && !(t->ContentX >= 480))
	{
		// render the window content
		if (t->Settings->Transparent)
		{
			if (t->Settings->Borders)
				t->Screen->AlphaBlitToScreen(t->ContentX, t->ContentY);
			else
				t->Screen->AlphaBlitToScreen(t->ContentX, t->ContentY - t->Topbar->Height);
		}
		else
		{
			width = 0; height = 0; offsetx = 0; offsety = 0, posx = t->ContentX, posy = t->ContentY;

			// calculate size to render
			if (t->Screen->Width + t->ContentX > 480)
				width = t->Screen->Width - ((t->Screen->Width + t->ContentX) - 480);
			if (t->X < 0)
			{
				width = t->Screen->Width + t->ContentX;
				offsetx = -t->X;
				posx = 0;
			}
			if (t->Screen->Height + t->ContentY > 272)
				height = t->Screen->Height - ((t->Screen->Height + t->ContentY) - 272);
			if (t->ContentY < 0)
			{
				height = t->Screen->Height + t->ContentY;
				offsety = -t->ContentY;
				posy = 0;
			}
			
			if (width && height)
				t->Screen->BlitToScreen(width, height, offsetx, offsety, posx, posy);
			if (width && !height)
				t->Screen->BlitToScreen(width, t->Screen->Height, offsetx, offsety, posx, posy);
			if (!width && height)
				t->Screen->BlitToScreen(t->Screen->Width, height, offsetx, offsety, posx, posy);
			if (!width && !height)
				t->Screen->BlitToScreen(t->Screen->Width, t->Screen->Height, offsetx, offsety, posx, posy);
		}
				
		if (t->Settings->Borders)
		{
			// render topbar
			t->Topbar->AlphaBlitToScreen(t->Topbar->Width, t->Topbar->Height, 0, 0, t->ContentX, t->Y);
			// render shadows
			_shadowVerticalRight->AlphaBlitToScreen(_shadowVerticalRight->Width, t->Height, 0, 0, t->ContentX + t->Width, t->Y);
			_shadowVerticalLeft->AlphaBlitToScreen(_shadowVerticalLeft->Width, t->Height, 0, 0, t->ContentX - _shadowVerticalLeft->Width, t->Y);
			_shadowHorizontal->AlphaBlitToScreen(t->Width, _shadowHorizontal->Height, 0, 0, t->ContentX, t->Y + t->Height);
			_shadowCornerRight->AlphaBlitToScreen(_shadowCornerRight->Width, _shadowCornerRight->Height, 0, 0, t->ContentX + t->Width, t->Y + t->Height);
			_shadowCornerLeft->AlphaBlitToScreen( _shadowCornerLeft->Width, _shadowCornerLeft->Height, 0, 0, t->ContentX - _shadowCornerLeft->Width, t->Y + t->Height);
		}
	}
}

Image* WindowManager::RenderWindowToImage(WindowApplication* t)
{
	Image* destination = new Image(_shadowVerticalRight->Width + _shadowVerticalLeft->Width + t->Width,
				       _shadowHorizontal->Height + t->Height);
			
	if (t->Settings->Borders)
	{
		t->Screen->BlitToImage(_shadowVerticalRight->Width, t->Topbar->Height, destination);
		// render topbar
		t->Topbar->BlitToImage(_shadowVerticalLeft->Width, 0, destination);
		// render shadows
		_shadowVerticalRight->BlitToImage(_shadowVerticalRight->Width, t->Height, 0, 0, _shadowVerticalLeft->Width + t->Width, 0, destination);
		_shadowVerticalLeft->BlitToImage(_shadowVerticalLeft->Width, t->Height, 0, 0, 0, 0, destination);
		_shadowHorizontal->BlitToImage(t->Width, _shadowHorizontal->Height, 0, 0, _shadowCornerLeft->Width, t->Height, destination);
		_shadowCornerRight->BlitToImage(_shadowCornerRight->Width, _shadowCornerRight->Height, 0, 0, _shadowVerticalLeft->Width + t->Width, t->Height, destination);
		_shadowCornerLeft->BlitToImage( _shadowCornerLeft->Width, _shadowCornerLeft->Height, 0, 0, 0, t->Height, destination);
	}
	return destination;
}
