TARGET = core
OBJS_CORE = ./core/main.o ./core/ASCore.o ./core/ASFileIO.o ./core/ASDesktop.o ./core/ASTaskbar.o \
		./core/ASUSBManager.o ./core/ASWiFiManager.o ./core/ASIconManager.o ./core/ASWindow.o \
		./core/ASWindowApplication.o ./core/ASWindowManager.o
OBJS_INPUT = ./input/ASController.o ./input/ASCursor.o
OBJS_GRAPHICS = ./graphics/ASImage.o ./graphics/ASGraphics.o ./graphics/ASFont.o
OBJS_CONFIG = ./config/ASConfig.o ./config/ASConfigTheme.o ./config/ASConfigIcons.o
OBJS_APPS = ./applications/Console.o
OBJS = $(OBJS_CORE) $(OBJS_GRAPHICS) $(OBJS_CONFIG) $(OBJS_INPUT) $(OBJS_APPS)

USE_PSPSDK_LIBC = 1

BUILD_PRX = 1

INCDIR = ./graphics ./core ./config ./input ./utils ./applications
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -I./
ASFLAGS = $(CFLAGS) -c -g

LIBDIR =
LDFLAGS = 

LIBS = -lpspgu -lpsprtc -lpng -ljpeg -lz -lpsphprm -lpspusb -lpspusbstor -lpspumd -lpspaudiolib -lpspaudio -lmad \
	   -lstdc++ -lpspatrac3 -lvorbisidec -lm -lc
	
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak