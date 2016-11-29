#include "artillery.h"
#include <psppower.h>

Image* LCD::_buffer;
Image* LCD::_bg;
list<string> LCD::_textBuffer;
list<LCDScreen*> LCD::_screens;
pspTime* LCD::_timed;
int LCD::_lastTime;
int LCD::_lastMinute;
int LCD::_init = 0;
int LCD::_pause = 0;
int LCD::_message = 0;
int LCD::_showmode = 0;
Color LCD::_textColor;
list<LCDScreen*>::iterator LCD::_screen;
int textoffsety = 0;

void LCD::Init()
{
	_bg = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_lcd").c_str());
	_buffer = Graphics::CreateImage(_bg->imageWidth, _bg->imageHeight);
	_lastTime = -1;
	_init = 1;
	string intro = "+++ Artillery 0.2 +++";
	_textBuffer.push_back(intro);
	_timed = (pspTime*) malloc(sizeof(pspTime));
	_textColor = 0xDDFFDFA2; /*0xAAFFBF92*/
	_lastMinute = -1;
	Core::CfgTheme->GetIntValue("lcdtextoffsety", textoffsety);
}

void LCD::Destroy()
{
	Graphics::FreeImage(_buffer);
	Graphics::FreeImage(_bg);
	_screens.clear();
}

void LCD::PrintText(string text)
{
	_textBuffer.push_back(text);
}

void LCD::PrintMessage(string text)
{
	_textBuffer.push_front(text);
	_lastTime = -10;
}

void LCD::PauseRotation()
{
	_pause = 1;
}

void LCD::ResumeRotation()
{
	_screens.pop_front();
	_pause = 0;
	_lastTime = -10;
}

void LCD::AddScreen(LCDScreen* screen)
{
	_screens.push_front(screen);
	_screen = _screens.begin();
}

void LCD::RemoveScreen(LCDScreen* screen)
{
	_screens.erase(find(_screens.begin(),_screens.end(),screen));
	if (_screens.empty())
		_screen = NULL;
	else
		_screen = _screens.begin();
}

void LCD::FocusScreen(LCDScreen* screen)
{
	_screen = find(_screens.begin(),_screens.end(),screen);
}

void LCD::NextScreen()
{
	if (_screens.end() != _screen)
		_screen++;
	else
		_screen = _screens.begin();
}

string LCD::GetDayOfTheWeek(int day, int month, int year)
{
	int a = (14 - month) / 12;
   int y = year - a;
   int m = month + 12 * a - 2;
   int d = (day + y + (y / 4) - (y / 100) + (y / 400) + ((31 * m) / 12))  % 7;
   switch (d)
   {
   		case 0:
   			return "Sunday.";
   			break;
   		case 1:
   			return "Monday.";
   			break;
   		case 2:
   			return "Tuesday.";
   			break;
   		case 3:
   			return "Wednesday.";
   			break;
   		case 4:
   			return "Thursday.";
   			break;
   		case 5:
   			return "Friday.";
   			break;
   		case 6:
   			return "Saturday.";
   			break;
   }
   return NULL;
}

string LCD::GetTime()
{
	char buf[20];
	sprintf(buf, "%02d:%02d",_timed->hour,_timed->minutes);
	string tmp;
	tmp.append(buf);
	return tmp;
}

string LCD::GetDate()
{
	char buf[20];
	string tmp = GetDayOfTheWeek((int) _timed->day, (int) _timed->month, (int) _timed->year);
	sprintf(buf, "%02d.%02d.%02d",_timed->day,_timed->month, _timed->year);
	tmp.append(buf);
	return tmp;
}

void LCD::RenderLCD()
{
	if (!_textBuffer.empty())
	{
		_message = 1;
		int center = (_buffer->imageWidth / 2) - ((_textBuffer.front().length() * 6) / 2);
		Graphics::BlitAlphaImageToImage(0, 0, _bg->imageWidth, _bg->imageHeight, _bg, 0, 0, _buffer);
		Font::PrintImage(_textBuffer.front().c_str(), center, textoffsety, _buffer, _textColor, 3);
		if (_pause != 1)
			_textBuffer.pop_front();
		_lastTime = _timed->seconds;
	}
	else
	{
		_lastTime = -10;
		_message = 0;
	}
}

void LCD::RenderDateTime()
{
	// blit background to buffer
	Graphics::BlitImageToImage(0, 0, _bg->imageWidth, _bg->imageHeight, _bg, 0, 0, _buffer);
		
	Font::PrintImage(GetDate().c_str(), 5, textoffsety, _buffer, _textColor, 3);
	Font::PrintImage(GetTime().c_str(), 150, textoffsety, _buffer, _textColor, 3);
	_lastMinute = _timed->minutes;
}

void LCD::RenderStatusBattery()
{
	// blit background to buffer
	Graphics::BlitAlphaImageToImage(0, 0, _bg->imageWidth, _bg->imageHeight, _bg, 0, 0, _buffer);
	
	int bp = scePowerGetBatteryLifePercent() / 10;
	string out = "Battery: ";
	for (int i=0; i < bp; i++)
		out += "i";
	Font::PrintImage(out.c_str(), 5, textoffsety, _buffer, _textColor, 3);
	if (scePowerIsBatteryCharging() == 1)
		Font::PrintImage("Charging", 85, textoffsety, _buffer, _textColor, 3);
	else
		Font::PrintImage("Not Charging", 85, textoffsety, _buffer, _textColor, 3);
	
}

void LCD::RenderStatusCPU()
{
	// blit background to buffer
	Graphics::BlitAlphaImageToImage(0, 0, _bg->imageWidth, _bg->imageHeight, _bg, 0, 0, _buffer);
	string out;
	out = "CPU: ";
	switch (scePowerGetCpuClockFrequencyInt())
	{
		case 333:
			out += "333";
			break;
		case 222:
			out += "222";
			break;
	}
	out += " MHZ   BUS: ";
	switch (scePowerGetBusClockFrequencyInt())
	{
		case 166:
			out += "166";
			break;
		case 111:
			out += "111";
			break;
	}
	out += " MHZ";
	Font::PrintImage(out.c_str(), 5, textoffsety, _buffer, _textColor, 3);
}

void LCD::RenderStatusConnections()
{
	// blit background to buffer
	Graphics::BlitAlphaImageToImage(0, 0, _bg->imageWidth, _bg->imageHeight, _bg, 0, 0, _buffer);
	string out;
	out = "USB: ";
	if (USBManager::IsEnabled())
		out += "ON";
	else
		out += "OFF";
	out += "  WIFI: ";
	if (WiFiManager::IsConnected() == 1)
		out += "ON";
	else
		out += "OFF";
	
	Font::PrintImage(out.c_str(), 5, textoffsety, _buffer, _textColor, 3);
}

void LCD::RenderStatusTheme()
{
	// blit background to buffer
	Graphics::BlitAlphaImageToImage(0, 0, _bg->imageWidth, _bg->imageHeight, _bg, 0, 0, _buffer);
	
	string out = "Theme: " + Core::Cfg->GetValue("theme");
	Font::PrintImage(out.c_str(), 5, textoffsety, _buffer, _textColor, 3);
}

void LCD::SwitchMode()
{
	_showmode += 1;
	if (_showmode == 5)
		_showmode = 0;
}

Image* LCD::GetLCDScreen()
{
	if (_init == 1)
	{
		sceRtcGetCurrentClockLocalTime(_timed);
			
		if (!_textBuffer.empty() || _message == 1)
		{
			if (_lastTime < 55 && _lastTime + 5 < _timed->seconds)
				RenderLCD();
			else if (_lastTime > 54 && _timed->seconds > (_lastTime - 55) && _timed->seconds < 55)
				RenderLCD();
			return _buffer;
		}
		else if (!_screens.empty())
			return ((LCDScreen*)*_screen)->GetLCDScreen();
		else
		{
			if (_showmode == 0)
				RenderDateTime();
			else if (_showmode == 1)
				RenderStatusBattery();
			else if (_showmode == 2)
				RenderStatusCPU();
			else if (_showmode == 3)
				RenderStatusConnections();
			else if (_showmode == 4)
				RenderStatusTheme();
		}
	}
	else
		return NULL;
		
	return _buffer;
}

