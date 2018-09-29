include /usr/src/uk/ssp_base.mk

CC_LINK     := $(CC_LINK)
CC_LINK_D   := $(CC_LINK_D)

CC_TARGET   = sys_watch
CC_TARGET_D = sys_watchd
INSTALL_PATH= ../../../../../out/bin
INSTALL_PATH_D= ../../../../../out_debug/bin
CC_RPATH    := -Wl,-rpath,.:../lib

SRCS =  ../../*.cpp
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
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH_D)/

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
			$(CP) $(CC_TARGET_D) $(INSTALL_PATH_D)/$(CC_TARGET_D); \
		fi

