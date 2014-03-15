#ifndef SCREENDRIVER_H
#define SCREENDRIVER_H

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272
#define SCREEN_LINE_SIZE 512

#include "Includes.h"
#include "Driver.h"

enum ScreenMode {Character, Graphical};

/** Screen Driver class
 */
class ScreenDriver : public Driver
{
	public:
		/** Singleton pattern; returns the instance of this class */
		static ScreenDriver* Instance();
		
		/** Called every core tick to update the driver if needed */
		void Tick();
		
		/** Sets the screen mode in which content is displayed */
		int SetScreenMode(ScreenMode);
		
		/** Returns the current screen mode */
		ScreenMode GetScreenMode();
		
		void Print(const char* format, ...);
		
		Color* GetVramBufferPointer();
		
		Color* GetVramDisplayPointer();
		
		unsigned int* GetGUListPointer();
		
		int GetScreenLineSize();
		
	private:
		ScreenDriver();
		~ScreenDriver();
		void InitGPU();
		void InitConsole();
		
	private:
		static ScreenDriver* _instance;
		ScreenMode _currentScreenMode;
		bool _GPUInitialized;
		int _screenWidth;
		int _screenHeight;
		int _screenLineSize;
		int _screenBuffNum;
		int _frameBufferSize;
		Color* _vramBase;
		unsigned int __attribute__((aligned(16))) _gulist[262144];
};

#endif
