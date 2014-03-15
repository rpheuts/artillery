#ifndef INCLUDES_H_
#define INCLUDES_H_

#define CFG_FILE "ms0:/Artillery/config/main.cfg"
#define THEME_PATH "ms0:/Artillery/themes/"
#define THEME_FILE "/theme.cfg"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 230

#include <psptypes.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <string>
#include <list>
using namespace std;

typedef u32 Color;

#include "Driver.h"
#include "Module.h"
#include "ScreenDriver.h"
#include "USBDriver.h"
#include "ControllerDriver.h"
#include "File.h"
#include "Config.h"
#include "ConfigTheme.h"
#include "Image.h"
#include "Graphics.h"
#include "Controller.h"
#include "WindowApplication.h"
#include "Interface.h"
#include "WindowManager.h"
#include "ArtilleryWindowManager.h"
#include "Desktop.h"
#include "Menu.h"
#include "Core.h"

#endif
