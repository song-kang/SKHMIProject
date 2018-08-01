include /usr/src/uk/ssp_base.mk

CC_INCLUDES := $(CC_INCLUDES) 
CC_LINK     := $(CC_LINK)  
CC_LINK_D   := $(CC_LINK_D) 

CC_TARGET   = sys_unit_agent
CC_TARGET_D = sys_unit_agentd
INSTALL_PATH= ../../lib/linux
CC_RPATH    := -Wl,-rpath,.:../lib

SRCS =  *.cpp
OBJS =  *.o


all:
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE)
		$(CC)  $(CC_RPATH) -o $(CC_TARGET) -g $(OBJS) $(CC_LINK)
		$(RM) *.o
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/
debug:
		$(CC) -c $(SRCS) $(CC_INCLUDES) $(CC_COMPLIE_D)
		$(CC)  $(CC_RPATH) -o $(CC_TARGET_D) -g $(OBJS) $(CC_LINK_D)
		$(RM) *.o
		$(CP) $(CC_TARGET_D) $(INSTALL_PATH)/

clean:
		$(RM) *.o
		$(RM) *.d
		$(RM) $(CC_TARGET)

install:
		$(CP) $(CC_TARGET) $(INSTALL_PATH)/$(CC_TARGET)
