#include "artillery.h"

LCDScreen::LCDScreen()
{
	_bg = Graphics::LoadPNG(Core::CfgTheme->GetPathValue("c_lcd").c_str());
	_buffer = Graphics::CreateImage(_bg->imageWidth, _bg->imageHeight);
	_timed = (pspTime*) malloc(sizeof(pspTime));
	_init = 1;
	_lastTime = -1;
	_textoffsety = Core::CfgTheme->GetIntValue("lcdtextoffsety");
	_textColor = 0xDDFFDFA2; /*0xAAFFBF92*/
	_default = "";
}

LCDScreen::~LCDScreen()
{
	Graphics::FreeImage(_bg);
	Graphics::FreeImage(_buffer);
	_textBuffer.clear();
}

void LCDScreen::PrintText(string text)
{
	LCDScreen::_textBuffer.push_back(text);
}

void LCDScreen::PrintMessage(string text)
{
	LCDScreen::_textBuffer.push_front(text);
	LCDScreen::_lastTime = -10;
}

void LCDScreen::SetDefaultText(string text)
{
	_default = text;
}

void LCDScreen::PauseRotation()
{
	_pause = 1;
}

void LCDScreen::ResumeRotation()
{
	_pause = 0;
	_lastTime = -10;
	RenderLCD();
}

void LCDScreen::RenderLCD()
{
	// blit background to buffer
	Graphics::BlitImageToImage(0, 0, _bg->imageWidth, _bg->imageHeight, _bg, 0, 0, _buffer);

	if (!_textBuffer.empty())
	{
		int center = (_buffer->imageWidth / 2) - ((_textBuffer.front().length() * 6) / 2);
		Font::PrintImage(_textBuffer.front().c_str(), center, _textoffsety, _buffer, _textColor, 3);
		if (_pause != 1)
			LCDScreen::_textBuffer.pop_front();
		_lastTime = _timed->seconds;
	}
	else
	{
		int center = (_buffer->imageWidth / 2) - ((_textBuffer.front().length() * 6) / 2);
		Font::PrintImage(_default.c_str(), center, _textoffsety, _buffer, _textColor, 3);
		_lastTime = -10;
	}
}

Image* LCDScreen::GetLCDScreen()
{
	if (_init == 1)
	{
		sceRtcGetCurrentClockLocalTime(_timed);
		
		if (_lastTime < 55 && _lastTime + 5 < _timed->seconds)
			RenderLCD();
		else if (_lastTime > 54 && _timed->seconds > (_lastTime - 55) && _timed->seconds < 55)
			RenderLCD();
	}
	else
		return NULL;
	
	return _buffer;
}
