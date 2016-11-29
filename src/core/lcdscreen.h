#ifndef LCDSCREEN_H
#define LCDSCREEN_H

#include "artillery.h"
#include <psprtc.h>
#include <list>

class LCDScreen
{
	public:
		LCDScreen();
		~LCDScreen();
		void PrintText(string text);
		void PrintMessage(string text);
		void SetDefaultText(string text);
		void PauseRotation();
		void ResumeRotation();
		Image* GetLCDScreen();
		
	protected:
		void RenderLCD();
	
	protected:
		Image* _buffer;
		Image* _bg;
		list<string> _textBuffer;
		string _default;
		pspTime* _timed;
		int _lastTime;
		int _init;
		int _pause;
		int _textoffsety;
		Color _textColor;
};

#endif
