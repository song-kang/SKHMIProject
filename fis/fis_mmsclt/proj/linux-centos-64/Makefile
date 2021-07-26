include /usr/src/uk/ssp_base.mk

CC_TARGET    = fis_mmsclt
CC_TARGET_D  = fis_mmscltd
INSTALL_PATH = ../../../../../out/bin
INSTALL_PATH_D = ../../../../../out_debug/bin
CC_INCLUDES := $(CC_INCLUDES) -I../../../../../src

RM = rm -rf
LN = ln
CP = cp -f
CC_LINK     := $(CC_LINK) -lSMmsClient 
CC_LINK_D   := $(CC_LINK_D) -lSMmsClientd

SRCS =  \
	../../main.cpp \
	../../MMSCLTApp.cpp \
	../../MMSClient.cpp \
	../../MMSClientMainStation.cpp \
	../../MMSClientSubStation.cpp \
	../../MMSClientSubStationNW.cpp \
	../../NaModel.cpp \
	../../../../public/UK9010App.cpp

OBJS =  \
		main.o \
		MMSCLTApp.o \
		MMSClient.o \
		MMSClientMainStation.o \
		MMSClientSubStation.o \
		MMSClientSubStationNW.o \
		UK9010App.o \
		NaModel.o

all:
		$(RM) debug
		$(CC) -c $(SRCS) $(CC_INCLUDES) -DNDEBUG $(CC_COMPLIE)
		$(CC) $(CC_RPATH) -o $(CC_TARGET) $(OBJS) $(CC_LINK)
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/

debug:
		$(RM) debug
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE_D)
		$(CC) $(CC_RPATH) -o $(CC_TARGET_D) -g $(OBJS) $(CC_LINK_D)
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH_D)/

clean:
		$(RM) *.o
		$(RM) $(CC_TARGET) $(CC_TARGET_D)

install:
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/$(CC_TARGET)
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH_D)/$(CC_TARGET_D)
