TARGET = core
OBJS = main.o core.o graphics.o framebuffer.o desktop.o taskbar.o font.o \
controller.o cursor.o windowmanager.o console.o interface.o \
iconmanager.o usbmanager.o modulemanager.o slideshow.o pluginmanager.o  \
mp3player.o fileio.o reader.o homebrewmanager.o panelmanager.o playbackmp3.o \
config.o lcd.o lcdscreen.o wifimanager.o stubs.o configtheme.o confighomebrew.o \
popuptheme.o


USE_PSPSDK_LIBC = 1

BUILD_PRX=1

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR =
LIBS= -lpspgu -lpsprtc -lpsppower -lpng -ljpeg -lz -lm -lc -lpspusb -lpspusbstor -lmad -lpspaudiolib -lpspaudio -lpspumd -lstdc++ -lpsphprm \
-lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility -lpspuser -lpspwlan

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
