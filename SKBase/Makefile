SBASE_DIR   := /home/uk

CC_INCLUDES := -I./ -I../include/SKBase -I/usr/include -I/usr/local/include -I/usr/include/mysql -I$(SBASE_DIR)/include/db/oracle/oci/include -I$(SBASE_DIR)/include  -I$(SBASE_DIR)/include/comm -I$(SBASE_DIR)/include/db -I$(SBASE_DIR)/include/db/mysql -I$(SBASE_DIR)/include/db/oracle -I$(SBASE_DIR)/include/db/mdb/  -I$(SBASE_DIR)/include/gui  -I$(SBASE_DIR)/include/gui/qt  -I$(SBASE_DIR)/include/gui/svg -I$(SBASE_DIR)/include/print -I$(SBASE_DIR)/include/print/usb_linux

CC_FLAGS    := -pipe -w -O3 -pipe 
CC_FLAGS_D  := -pipe -Wall -O2 -g -pipe
CC_COMPLIE  := $(CC_FLAGS) $<
CC_COMPLIE_D:= $(CC_FLAGS_D) $<
CC_LINK     := -L/usr/lib/ -L/usr/local/lib/ -L$(SBASE_DIR)/lib -lsbase -lpthread -lm -lrt
CC_LINK_D   := -L/usr/lib/ -L/usr/local/lib/ -L$(SBASE_DIR)/lib -lsbased -lpthread -lm -lrt

CC := g++
AR = ar cqs
RM := rm -rf
LN := ln
CP := cp -f

CC_TARGET   = libSKBase.so
CC_TARGET_D = libSKBased.so
INSTALL_PATH= /home/uk/lib

SRCS =  *.cpp
OBJS =  *.o 

all:
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE)
		$(RM) $(CC_TARGET)
		$(CC) -shared $(OBJS) -o $(CC_TARGET)
		$(RM) *.o
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/

debug:
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE_D)
		$(RM) $(CC_TARGET)
		$(CC) -shared $(OBJS) -o $(CC_TARGET_D)
		$(RM) *.o
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH)/

clean:
		$(RM) *.o
		$(RM) $(CC_TARGET)

install:
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/$(CC_TARGET)
