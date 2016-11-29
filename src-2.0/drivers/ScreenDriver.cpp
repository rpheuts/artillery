#include <stdarg.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include "ScreenDriver.h"

ScreenDriver* ScreenDriver::_instance = 0;

ScreenDriver* ScreenDriver::Instance()
{
	if (_instance == 0)
		_instance = new ScreenDriver();
	return _instance;
}

ScreenDriver::ScreenDriver() : Driver(1, "Screen", true)
{
	_GPUInitialized = false;
	_currentScreenMode = Character;
	_status = DriverRunning;
}

ScreenDriver::~ScreenDriver()
{
	
}

void ScreenDriver::Tick()
{
	sceDisplayWaitVblankStart();
	if (_currentScreenMode == Graphical && _GPUInitialized)
	{
		sceGuSwapBuffers();
		_screenBuffNum ^= 1;
   
		sceGuClearColor(0);
		sceGuClear(GU_COLOR_BUFFER_BIT);
	}
}
		
int ScreenDriver::SetScreenMode(ScreenMode mode)
{
	_currentScreenMode = mode;
	
	if (_currentScreenMode == Character)
	{
		InitConsole();
		return 0;
	}
	else if (_currentScreenMode == Graphical && !_GPUInitialized)
	{
		InitGPU();
		_GPUInitialized = true;
		return 0;
	}
	return -1;
}
		
ScreenMode ScreenDriver::GetScreenMode()
{
	return _currentScreenMode;
}

void ScreenDriver::Print(const char *format, ...)
{
	if (_currentScreenMode == Character)
	{
		// Assemble the parameters given and pass it to the PSPSDK printf function 
		va_list	opt;
		char     buff[2048];
		int		bufsz;
   
		va_start(opt, format);
		bufsz = vsnprintf( buff, (size_t) sizeof(buff), format, opt);
		pspDebugScreenPrintData(buff, bufsz);
	}
}

Color* ScreenDriver::GetVramBufferPointer()
{
	Color* vram = (Color*) _vramBase;
	if (_screenBuffNum == 0) vram += _frameBufferSize / sizeof(Color);
	return vram;
}

Color* ScreenDriver::GetVramDisplayPointer()
{
	Color* vram = (Color*) _vramBase;
	if (_screenBuffNum == 1) vram += _frameBufferSize / sizeof(Color);
	return vram;
}

unsigned int* ScreenDriver::GetGUListPointer()
{
	return (unsigned int*)&_gulist;
}
		
int ScreenDriver::GetScreenLineSize()
{
	return _screenLineSize;
}

void ScreenDriver::InitConsole()
{
	pspDebugScreenInit();
	pspDebugScreenClear();
}

void ScreenDriver::InitGPU()
{
	// Initialize
	_screenWidth = SCREEN_WIDTH;
	_screenHeight = SCREEN_HEIGHT;
	_screenLineSize = SCREEN_LINE_SIZE;
	_screenBuffNum = 0;
	_frameBufferSize = _screenLineSize*_screenHeight*4;
	_vramBase = (Color*) (0x40000000 | 0x04000000);

	sceDisplaySetMode(0, _screenWidth, _screenHeight);
	sceGuInit();

	// Setup
	sceGuStart(GU_DIRECT,_gulist);
	
	sceGuDrawBuffer(GU_PSM_8888, (void*)_frameBufferSize, _screenLineSize);
	sceGuDispBuffer(_screenWidth, _screenHeight, (void*)0, _screenLineSize);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
	sceGuDepthBuffer((void*) (_frameBufferSize*2), _screenLineSize);
	sceGuOffset(2048 - (_screenWidth / 2), 2048 - (_screenHeight / 2));
	sceGuViewport(2048, 2048, _screenWidth, _screenHeight);
	sceGuDepthRange(0xc350, 0x2710);
	sceGuScissor(0, 0, _screenWidth, _screenHeight);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuAlphaFunc(GU_GREATER, 0, 0xff);
	sceGuEnable(GU_ALPHA_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuTexMode(GU_PSM_8888, 0, 0, 0);
	sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	sceGuAmbientColor(0xffffffff);
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	sceGuFinish();
	sceGuSync(0, 0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}
