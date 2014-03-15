#include "Includes.h"

#define DEFAULT_WINDOW_POSITION_X 40
#define DEFAULT_WINDOW_POSITION_Y 10

ArtilleryWindowManager* ArtilleryWindowManager::_artilleryInstance = 0;

ArtilleryWindowManager* ArtilleryWindowManager::Instance()
{
	if (_artilleryInstance == 0)
		_artilleryInstance = new ArtilleryWindowManager();
	return _artilleryInstance;
}

ArtilleryWindowManager::ArtilleryWindowManager() : WindowManager()
{
	_currentMain = 0;
	_previousMain = 0;
	_currentFade = 0;
	_currentFadeAmount= 255;
}

ArtilleryWindowManager::~ArtilleryWindowManager()
{
	delete (_savedScreenState);
	
}

void ArtilleryWindowManager::Tick()
{
	// If there is no Artillery window available just hand control to the base WindowManager
	if (!_currentMain)
	{
		if (!_controller->Handled)
			this->HandleController();
		this->Render();
	}
	else
	{
		// Else we take control and do the rendering ourselves
		_savedScreenState->BlitToScreen(0, 0);
		this->HandleMainWindow();
		if (_currentMain)
			this->RenderMainWindow();
	}
	
	// Fade a snaphot of the last window
	if (_currentFade)
	{
		_currentFade->ApplyEffect(ImageFade, -15);
		if ((_currentFadeAmount -= 15) < 0)
		{
			delete (_currentFade);
			_currentFadeAmount = 255;
			_currentFade = 0;
			_currentMain = 0;
		}
		else
			_currentFade->AlphaBlitToScreen(DEFAULT_WINDOW_POSITION_X - _shadowVerticalRight->Width, DEFAULT_WINDOW_POSITION_Y);
	}
}

void ArtilleryWindowManager::HandleMainWindow()
{
	// Check if the Start button was pressed to switch back to the menu
	if (Controller::Instance()->IsPressed(PSP_CTRL_START))
	{
		_currentMain->Settings->Enabled = 0;
		_currentFade = _currentMain->RenderToImage();
		return;
	}
	if (Controller::Instance()->IsPressed(PSP_CTRL_RTRIGGER) && !Controller::Instance()->WasPressed(PSP_CTRL_RTRIGGER))
		ShowPreviousArtilleryWindow();
	if (Controller::Instance()->IsPressed(PSP_CTRL_LTRIGGER) && !Controller::Instance()->WasPressed(PSP_CTRL_LTRIGGER))
		ShowNextArtilleryWindow();
	
	// Handle controller
	if (!_currentMain->Settings->Fullscreen)
		_currentMain->HandleController(_controller->CursorX - _currentMain->ContentX, _controller->CursorY - _currentMain->ContentY);
	else
		_currentMain->HandleController(_controller->CursorX, _controller->CursorY);
	_controller->Handled = 1;
}

void ArtilleryWindowManager::RenderMainWindow()
{
	if (_currentMain->Settings->Enabled)
	{
		// Slide screen
		if (_currentMain->X < DEFAULT_WINDOW_POSITION_X)
			_currentMain->Position(_currentMain->X + 20, _currentMain->Y);
		if (_currentMain->X > DEFAULT_WINDOW_POSITION_X)
			_currentMain->Position(_currentMain->X - 20, _currentMain->Y);
		
		// Slide old screen
		if (_previousMain)
		{
			if (_previousMain->X > -_previousMain->Width)
			{
				_previousMain->Position(_previousMain->X - 20, _previousMain->Y);
				this->RenderWindow(_previousMain);
			}
			else
				_previousMain = 0;
		}
		
		// Render the contents and then the window
		_currentMain->Render();
		this->RenderWindow(_currentMain);
	}
}

void ArtilleryWindowManager::ShowNextArtilleryWindow()
{
	if (_mainWindows.size() > 1)
	{
		list<WindowApplication*>::iterator windowIt = _mainWindows.begin();
		ShowArtilleryWindow((WindowApplication*)*++windowIt);
	}
}

void ArtilleryWindowManager::ShowPreviousArtilleryWindow()
{
	if (_mainWindows.size() > 1)
	{
		ShowArtilleryWindow(_mainWindows.back());
	}
}

void ArtilleryWindowManager::ShowArtilleryWindow(WindowApplication* window)
{
	_mainWindows.remove(window);
	_mainWindows.push_front(window);
	if (_currentMain)
		_previousMain = _currentMain;
	_currentMain = window;
	_currentMain->Position(SCREEN_WIDTH, DEFAULT_WINDOW_POSITION_Y);
	_currentMain->Settings->Enabled = 1;
}

int ArtilleryWindowManager::RegisterArtilleryWindow(WindowApplication* window)
{
	if (!_currentMain)
	{
		// Save the current screen
		_savedScreenState = Graphics::Instance()->SaveFramebuffer();
		_savedScreenState->ApplyEffect(ImageBlur);
	}
	
	if (window->X == -1)
		window->X = SCREEN_WIDTH;
	if (window->Y == -1)
		window->Y = DEFAULT_WINDOW_POSITION_Y;
	window->Topbar = InitTopbar(window->Width, window->Name);
	
	window->ContentX = window->X + _shadowVerticalLeft->Width;
	window->ContentY = window->Y + window->Topbar->Height;
	window->Height += window->Topbar->Height;
	
	_mainWindows.push_front(window);
	_currentMain = window;
	return 0;
}

void ArtilleryWindowManager::DestroyArtilleryWindow(WindowApplication* window)
{
	window->DestroyHandler();
	if (window->Screen)
		delete (window->Screen);
	delete (window->Topbar);
	
	_mainWindows.remove(window);
}
