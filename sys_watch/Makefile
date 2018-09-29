SBASE_DIR   := ../..

CC_INCLUDES := -I./ -I/usr/include -I/usr/local/include -I/usr/include/mysql -I$(SBASE_DIR)/include  -I$(SBASE_DIR)/include/comm -I$(SBASE_DIR)/include/db -I$(SBASE_DIR)/include/db/oracle -I$(SBASE_DIR)/include/db/oracle/oci/include -I$(SBASE_DIR)/include/db/mysql -I$(SBASE_DIR)/include/db/mdb/  -I$(SBASE_DIR)/include/gui  -I$(SBASE_DIR)/include/gui/qt  -I$(SBASE_DIR)/include/gui/svg -I$(SBASE_DIR)/include/print -I$(SBASE_DIR)/include/print/usb_linux -I../SKHMIProject/include/SKBase

CC_FLAGS    := -pipe -w -O3 -pipe 
CC_FLAGS_D  := -pipe -Wall -O2 -g -pipe
CC_COMPLIE  := $(CC_FLAGS) $<
CC_COMPLIE_D:= $(CC_FLAGS_D) $<

CC_LINK     := -L/usr/lib/ -L/usr/local/lib/ -L$(SBASE_DIR)/lib -L$(SBASE_DIR)/lib/instantclient_11_2 -lSKBase -lsbase_mdb -lsbase_mysql -lsbase_oracle -lsbase -lmysqlclient -lociei -lpthread -lm -lrt
CC_LINK_D   := -L/usr/lib/ -L/usr/local/lib/ -L$(SBASE_DIR)/lib -L$(SBASE_DIR)/lib/instantclient_11_2 -lSKBased -lsbase_mdbd -lsbase_mysqld -lsbase_oracled -lsbased -lmysqlclient -lociei -lpthread -lm -lrt

CC := g++
AR = ar cqs
RM := rm -rf
LN := ln
CP := cp -f

CC_TARGET   = sys_watch
CC_TARGET_D = sys_watchd
INSTALL_PATH= ../../bin
CC_RPATH    := -Wl,-rpath,.:../lib

SRCS =  *.cpp
OBJS =  *.o

all:
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE)
		$(CC) $(CC_RPATH) -o $(CC_TARGET) $(OBJS) $(CC_LINK)
		$(RM) *.o
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/
		
debug:
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE_D)
		$(CC) $(CC_RPATH) -o $(CC_TARGET_D) -g $(OBJS) $(CC_LINK_D)
		$(RM) *.o
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH)/

clean:
		$(RM) *.o
		@if test -f $(CC_TARGET); \
		then \
			$(RM) $(CC_TARGET); \
		fi
		@if test -f $(CC_TARGET_D); \
		then \
			$(RM) $(CC_TARGET_D); \
		fi

install:
		@if test -f $(CC_TARGET); \
		then \
			$(CP) $(CC_TARGET) $(INSTALL_PATH)/$(CC_TARGET); \
		fi
		@if test -f $(CC_TARGET_D); \
		then \
			$(CP) $(CC_TARGET_D) $(INSTALL_PATH)/$(CC_TARGET_D); \
		fi

