#include <string.h>
#include <pspctrl.h>

#include "Includes.h"

ASWindowManager* ASWindowManager::_instance = 0;

ASWindowManager* ASWindowManager::Instance()
{
	if (_instance == 0)
		_instance = new ASWindowManager();
	return _instance;
}

ASWindowManager::ASWindowManager()
{
	_createdLastX = 5;
	_createdLastY = 2;
	_titlebarTextOffsetX = 0;
	_titlebarTextOffsetY = 0;
	_graphics = ASGraphics::Instance();
	_controller = ASController::Instance();
	
	ASConfigTheme* mainCfg = Core::MainConfigTheme;
	_shadowHorizontal = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndborderhor"));
	_shadowVerticalRight = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndborderverright"));
	_shadowVerticalLeft = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndborderverleft"));
	_shadowCornerRight = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndbordercorright"));
	_shadowCornerLeft = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndbordercorleft"));
	_buttonClose = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndtopbarbtn"));
	_topbarLeft = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndtopbarleft"));
	_topbarRight = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndtopbarright"));
	_topbarCenter = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndtopbarcenter"));
	_topbarScaleLeft = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndtopbarscaleleft"));
	_topbarScaleRight = _graphics->LoadPNG(mainCfg->GetPathValue("c_wndtopbarscaleright"));
	mainCfg->GetIntValue("_titlebarTextOffsetX", _titlebarTextOffsetX);
	mainCfg->GetIntValue("_titlebarTextOffsetY", _titlebarTextOffsetY);
	mainCfg = 0;
}

ASWindowManager::~ASWindowManager()
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

void ASWindowManager::DestroyWindow(WindowApplication* window)
{
	window->Destroy();
	if (window->Screen)
		delete (window->Screen);
	delete (window->Topbar);
	
	_windows->remove(window);
}

void ASWindowManager::RegisterWindow(WindowApplication* window)
{
	window->X = _createdLastX;
	window->Y = _createdLastY;
	_windows->push_back(window);
	
	_createdLastX += 20;
	_createdLastY += 10;
}

ASImage* ASWindowManager::InitTopbar(int width, string text)
{
	int index = 0;
	ASImage* topbar =  new ASImage(width, _topbarLeft->Height);
	ASImage* tmp = 0;

	// Blit left part first
	_graphics->BlitImageToImage(0, 0, _topbarLeft->Width, _topbarLeft->Height, _topbarLeft, 0, 0, topbar);
	index += _topbarLeft->Width;


	// Blit scaling part for text
	tmp = _graphics->ResizeImage((text.length()*6)+_titlebarTextOffsetX, _topbarScaleLeft->Height, _topbarScaleLeft);
	_graphics->BlitImageToImage(0, 0, tmp->Width, tmp->Height, tmp, index, 0, topbar);
	index += tmp->Width;
	delete (tmp);


	// Blit center part
	_graphics->BlitImageToImage(0, 0, _topbarCenter->Width, _topbarCenter->Height, _topbarCenter, index, 0, topbar);
	index += _topbarCenter->Width;
	
	// Blit scaling right part
	//int scWidth = width - index - _topbarRight->Width;
	/*if (scWidth > 0)
	{
		tmp = _graphics->ResizeImage(120, _topbarScaleLeft->Height, _topbarScaleLeft);
		_graphics->BlitImageToImage(0, 0, tmp->Width, tmp->Height, tmp, index, 0, topbar);
		index += tmp->Width;
		delete (tmp);
	}*/
	
	// Blit last part
	_graphics->BlitImageToImage(0, 0, _topbarRight->Width, _topbarRight->Height, _topbarRight, index, 0, topbar);
	
	// Blit close button
	_graphics->BlitAlphaImageToImage(0, 0, _buttonClose->Width, _buttonClose->Height, _buttonClose, width - _buttonClose->Width - 3, 0, topbar);
	
	#ifdef WINDOW_TITLES
		// Print text
		Core::Fonts->PrintImage(text, _titlebarTextOffsetX, _titlebarTextOffsetY, topbar, 0xFF00D2FF, 0, 0);
	#endif
	return topbar;
}

void ASWindowManager::Focus(ASWindow* window)
{
	ASWindow* t = ListStart;
	
	if (window != t)
	{
		ASWindow* cur = NULL;
		ASWindow* prev= NULL;
		
		while (t && t->Next)
		{
			if (t->Next == window)
			{
				cur = t->Next;
				prev = t;
				break;
			}
			t = t->Next;
		}
		// if we are at the end of the list and there is a previous node set that node to be the end 
		if (cur == ListEnd && prev)
			ListEnd = prev;
		// set the next node of our previous node to our next
		prev->Next = cur->Next;
		// set the previous of our next node to our previous if there was any
		if (cur->Next)
			cur->Next->Previous = prev;
		// set our next node the to the original start of the list
		cur->Next = ListStart;
		// we are the first now, so we don't have a previous
		cur->Previous = NULL;
		// swap the previous node of our next node to our previous so we get rendered ;)
		cur->Next->Previous = cur;
		// set us to the static startnode
		ListStart = cur;
		// pfew.. done
	}
}

void ASWindowManager::WindowPosition(ASWindow* window, int x, int y)
{
	if (!window->Borders)
		y -= window->Topbar->Height;
	window->X = x;
	window->Y = y;
	window->Application->X = x;
	window->Application->Y = y;
	window->ContentX = x;
	window->ContentY = y + window->Topbar->Height;
}

void ASWindowManager::HandleFocus()
{
	if (_controller->LastInput == PSP_CTRL_CROSS && ListStart && !_isBeingDragged)
	{
		ASWindow* t = ListStart;
		while (t)
		{
			if (t->Enabled)
			{
				if (ASInterface::IsWithinWindow(_controller->AnalogX, _controller->AnalogY, t))
				{
					if (ListStart != t)
					{
						Focus(t);
						_isActive = 1;
						_controller->Handled = 1;
					}
					break;
				}
				else if (_controller->AnalogX < t->X || _controller->AnalogY < t->Y || _controller->AnalogX > (t->X + t->Width) || _controller->AnalogY > (t->Y + t->Height))
					_isActive = 0;
			}
			t = t->Next;
		}
	}
}

void ASWindowManager::HandleDrag()
{
	if (_controller->LastInput == PSP_CTRL_CROSS && ListStart && _controller->AnalogX > ListStart->X + ListStart->Width - (_buttonClose->Width + 3) && _controller->AnalogY > ListStart->Y && _controller->AnalogX < (ListStart->X + ListStart->Width) && _controller->AnalogY < (ListStart->Y + ListStart->Topbar->Height))
	{
				DestroyWindow(ListStart);
				_controller->Handled = 1;
	}
	if (_controller->LastInput == PSP_CTRL_CROSS && ListStart)
	{
		if (ASInterface::IsWithinImage(_controller->AnalogX, _controller->AnalogY, ListStart->X, ListStart->Y, ListStart->Topbar))
		{
			WindowPosition(ListStart, ListStart->X + (_controller->AnalogX - _controller->PreviousAnalogX), ListStart->Y + (_controller->AnalogY - _controller->PreviousAnalogY));
			_isBeingDragged = 1;
			_controller->Handled = 1;
			_isActive = 1;
		}
		else if (_controller->IsLastInputPressed && _controller->LastInput == PSP_CTRL_CROSS && ListStart && _isBeingDragged)
		{
			WindowPosition(ListStart, ListStart->X + (_controller->AnalogX - _controller->PreviousAnalogX), ListStart->Y + (_controller->AnalogY - _controller->PreviousAnalogY));
			_controller->Handled = 1;
			_isActive = 1;
		}
	}
	else if (_isBeingDragged)
		_isBeingDragged = 0;
}

void ASWindowManager::HandleController()
{
	if (ListStart)
	{
		if ((ASInterface::IsWithinWindowContent(_controller->AnalogX, _controller->AnalogY, ListStart) || _isActive) && ListStart->Enabled == 1)
		{
			if (!ListStart->Fullscreen)
				ListStart->HandleController(_controller->AnalogX - ListStart->ContentX, _controller->AnalogY - ListStart->ContentY, _controller->LastInput);
			else
				ListStart->HandleController(_controller->AnalogX, _controller->AnalogY, _controller->LastInput);
			_controller->Handled = 1;
			_isActive = 1;
		}
		else
			_isActive = 0;
	}
}

void ASWindowManager::Handle()
{
	if (!_controller->Handled)
	{
		HandleFocus();
		if (!_controller->Handled)
			HandleDrag();
		if (!_controller->Handled)
			HandleController();
	}
}

void ASWindowManager::Render()
{
	FullScreenEnabled = 0;
	ASWindow* ontop = 0;
	if (ListEnd)
	{
		ASWindow* t = ListEnd;
		while (t)
		{
			if (t->Enabled == 1)
			{
				t->Render();
				if (t->Fullscreen)
				{
					t->RenderFullscreen();
					t = t->Previous;
					FullScreenEnabled = 1;
					continue;
				}
				
				if (t->OnTop == 1)
				{
					ontop = t;
					t = t->Previous;
					continue;
				}
				RenderWindow(t);
			}
			t = t->Previous;
		}
		
		if (ontop)
			RenderWindow(ontop);
	}
}

void ASWindowManager::RenderWindow(ASWindow* t)
{
	int width, height, offsetx, offsety, posx, posy;
	
	if (!(t->ContentY >= 272) && !(t->ContentX >= 480))
	{
		width = 0; height = 0; offsetx = 0; offsety = 0, posx = t->ContentX, posy = t->ContentY;
				// render window content
				// calculate size to render
		if (t->Application->Screen->Width + t->ContentX > 480)
			width = t->Application->Screen->Width - ((t->Application->Screen->Width + t->ContentX) - 480);
		if (t->X < 0)
		{
			width = t->Application->Screen->Width + t->ContentX;
			offsetx = -t->X;
			posx = 0;
		}
		if (t->Application->Screen->Height + t->ContentY > 272)
			height = t->Application->Screen->Height - ((t->Application->Screen->Height + t->ContentY) - 272);
		if (t->ContentY < 0)
		{
			height = t->Application->Screen->Height + t->ContentY;
			offsety = -t->ContentY;
			posy = 0;
		}
				// render the window content
		if (t->Transparent)
			_graphics->BlitAlphaImageToScreen(0, 0, t->Application->Screen->Width, t->Application->Screen->Height, t->Application->Screen, t->ContentX, t->ContentY);
		else {
			if (width && height)
				_graphics->BlitImageToScreen(offsetx, offsety, width, height, t->Application->Screen, posx, posy);
			if (width && !height)
				_graphics->BlitImageToScreen(offsetx, offsety, width, t->Application->Screen->Height, t->Application->Screen, posx, posy);
			if (!width && height)
				_graphics->BlitImageToScreen(offsetx, offsety, t->Application->Screen->Width, height, t->Application->Screen, posx, posy);
			if (!width && !height)
				_graphics->BlitImageToScreen(offsetx, offsety, t->Application->Screen->Width, t->Application->Screen->Height, t->Application->Screen, posx, posy); }
				
			if (t->Borders)
			{
					// render topbar
				_graphics->BlitAlphaImageToScreen(0, 0, t->Topbar->Width, t->Topbar->Height, t->Topbar, t->X, t->Y);
					// render shadows
				_graphics->BlitAlphaImageToScreen(0,0 , _shadowVerticalRight->Width, t->Height, _shadowVerticalRight, t->ContentX + t->Width, t->Y);
				_graphics->BlitAlphaImageToScreen(0,0 , _shadowVerticalLeft->Width, t->Height, _shadowVerticalLeft, t->ContentX - _shadowVerticalLeft->Width, t->Y);
				_graphics->BlitAlphaImageToScreen(0,0 , t->Width, _shadowHorizontal->Height, _shadowHorizontal, t->ContentX, t->Y + t->Height);
				_graphics->BlitAlphaImageToScreen(0,0 , _shadowCornerRight->Width, _shadowCornerRight->Height, _shadowCornerRight, t->ContentX + t->Width, t->Y + t->Height);
				_graphics->BlitAlphaImageToScreen(0,0 , _shadowCornerLeft->Width, _shadowCornerLeft->Height, _shadowCornerLeft, t->ContentX - _shadowCornerLeft->Width, t->Y + t->Height);
			}
	}
}
