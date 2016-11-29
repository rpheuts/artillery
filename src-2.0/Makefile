TARGET = core
OBJS_CORE = ./core/Main.o ./core/Core.o
OBJS_DRIVERS = ./drivers/Driver.o ./drivers/ScreenDriver.o ./drivers/USBDriver.o ./drivers/ControllerDriver.o
OBJS_LIB = ./libs/File.o ./libs/Image.o ./libs/Graphics.o ./libs/Config.o ./libs/ConfigTheme.o \
		./libs/WindowApplication.o
OBJS_MOD = ./modules/Module.o ./modules/Controller.o ./modules/WindowManager.o ./modules/Desktop.o \
		./modules/ArtilleryWindowManager.o
OBJS_APP = ./applications/menu/Menu.o ./applications/menu/MenuButton.o ./applications/menu/MenuPopup.o

OBJS = $(OBJS_CORE) $(OBJS_DRIVERS) $(OBJS_LIB) $(OBJS_MOD) $(OBJS_APP)

USE_PSPSDK_LIBC = 1

BUILD_PRX = 1

INCDIR = ./core ./drivers ./libs ./modules ./utils ./applications/menu
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -I./
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LDFLAGS = 

LIBS = -lpspgu -lpsprtc -lpng -ljpeg -lz -lpsphprm -lpspusb -lpspusbstor -lpspumd -lpspaudiolib -lpspaudio -lmad \
	   -lstdc++ -lvorbisidec -lm -lc
	
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak