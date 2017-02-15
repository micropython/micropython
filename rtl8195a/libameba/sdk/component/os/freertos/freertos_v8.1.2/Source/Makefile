
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

MODULE_IFLAGS = -I./include


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = tasks.o list.o croutine.o queue.o timers.o event_groups.o 
ifeq ($(CONFIG_RELEASE_BUILD),y)
	OBJS = 
else
endif


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS COPY_RAM_OBJS
all: CORE_TARGETS COPY_RAM_OBJS
	make -C portable/MemMang all
	make -C portable/GCC/ARM_CM3 all
	

	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	$(REMOVE) portable/MemMang/*.o
	$(REMOVE) portable/MemMang/*.i
	$(REMOVE) portable/MemMang/*.s
	$(REMOVE) portable/MemMang/*.d
	$(REMOVE) portable/GCC/ARM_CM3/*.o
	$(REMOVE) portable/GCC/ARM_CM3/*.i
	$(REMOVE) portable/GCC/ARM_CM3/*.s
	$(REMOVE) portable/GCC/ARM_CM3/*.d
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	
-include $(DEPS)
