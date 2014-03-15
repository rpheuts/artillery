#ifndef ASINCLUDES_H_
#define ASINCLUDES_H_

#define DEBUG
#define VERSION "Artillery Shell Dev"
// #define WINDOW_TITLES

#define TRUE 1
#define FALSE 0

#define MK8888(b,g,r,a) (((b&0xFF)<<16) + ((g&0xFF)<<8) + ((r&0xFF)) + ((a&0xFF)<<24))
#define IS_ALPHA(color) ((color)&0xFF000000?0:1)
#define IS_RED(color) ((color)&0x000000FF?1:0)
#define GET_ALPHA(c) ((unsigned short)((c & 0xFF000000) >> 24))
#define GET_RED(c) ((unsigned short)((c & 0x000000FF)))
#define GET_GREEN(c) ((unsigned short)((c & 0x0000FF00) >> 8))
#define GET_BLUE(c) ((unsigned short)((c & 0x00FF0000)>>16))
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272
#define PSP_LINE_SIZE 512

#define CFG_FILE "ms0:/Artillery/config/main.cfg"
#define THEME_PATH "ms0:/Artillery/themes/"
#define THEME_FILE "/theme.cfg"

#include <psptypes.h>
#include <pspkernel.h>
#include <string>
#include <list>
using namespace std;

typedef u32 Color;
typedef Color* COLORREF;

#include "ASConfig.h"
#include "ASConfigTheme.h"
#include "ASConfigIcons.h"
#include "ASUSBManager.h"
#include "ASWiFiManager.h"
#include "ASFileIO.h"
#include "ASImage.h"
#include "ASGraphics.h"
#include "ASFont.h"
#include "ASController.h"
#include "ASCursor.h"
#include "ASWindowApplication.h"
#include "ASWindow.h"
#include "ASWindowManager.h"
#include "ASInterface.h"
#include "ASDesktop.h"
#include "ASTaskbar.h"
#include "ASIconManager.h"
#include "ASCore.h"

#endif
