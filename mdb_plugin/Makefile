include /usr/src/uk/ssp_base.mk
#include /usr/src/uk/oms.mk

CC_LINK     := $(CC_LINK)  -L/home/instantclient_12_1/ -lsbase -lsbase_oracle -locci -lclntsh -ldl -shared
CC_LINK_D   := $(CC_LINK_D) -L/home/instantclient_12_1/ -lsbased -lsbase_oracled -locci -lclntsh -ldl -shared

CC_TARGET   = libmdb_plugin.so
CC_TARGET_D   = libmdb_plugind.so
INSTALL_PATH= ../../../out/bin
INSTALL_PATH_D= ../../../out_debug/bin

SRCS =  *.cpp
OBJS =  *.o

all:
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE)
		$(CC) -o $(CC_TARGET) -g $(OBJS) $(CC_LINK)
		$(RM) *.o
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/
debug:
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE_D)
		$(CC) -o $(CC_TARGET_D) -g $(OBJS) $(CC_LINK_D)
		$(RM) *.o
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH_D)/

clean:
		$(RM) *.o
		$(RM) *.d
		$(RM) $(CC_TARGET)

install:
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/$(CC_TARGET)
