TARGET = bootloader
OBJS = bootloader.o

USE_PSPSDK_LIBC = 1

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = 

LIBS = -lpspusb -lpspusbstor
PSP_EBOOT_TITLE = Artillery
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Artillery-Dev

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
