
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

MODULE_IFLAGS = 


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = port.o
ifeq ($(CONFIG_RELEASE_BUILD),y)
	OBJS = 
else
endif


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS

	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

-include $(DEPS)
