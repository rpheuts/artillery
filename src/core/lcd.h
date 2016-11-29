#ifndef LCD_H
#define LCD_H

#include "artillery.h"
#include <psprtc.h>
#include <list>

class LCD
{
	public:
		static void Init();
		static void Destroy();
		static void PrintText(string text);
		static void PrintMessage(string text);
		static void PauseRotation();
		static void ResumeRotation();
		static void AddScreen(LCDScreen* screen);
		static void RemoveScreen(LCDScreen* screen);
		static void FocusScreen(LCDScreen* screen);
		static void NextScreen();
		static void SwitchMode();
		static Image* GetLCDScreen();
		
	private:
		static void RenderLCD();
		static void RenderDateTime();
		static void RenderStatusBattery();
		static void RenderStatusCPU();
		static void RenderStatusConnections();
		static void RenderStatusTheme();
		static string GetTime();
		static string GetDate();
		static string GetDayOfTheWeek(int day, int month, int year);
		
	private:
		static Image* _buffer;
		static Image* _bg;
		static list<string> _textBuffer;
		static list<LCDScreen*> _screens;
		static list<LCDScreen*>::iterator _screen;
		static pspTime* _timed;
		static int _lastTime;
		static int _lastMinute;
		static int _init;
		static int _message;
		static int _pause;
		static Color _textColor;
		static int _showmode;
};
#endif
