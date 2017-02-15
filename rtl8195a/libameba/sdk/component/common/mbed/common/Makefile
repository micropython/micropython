
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

MODULE_IFLAGS = -I../


ifeq ($(CONFIG_TOOLCHAIN_ARM_GCC), y)
GCC_PROJ_BASE = ../../../../../
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/sw/lib/sw_lib/mbed/hal/
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/sw/lib/sw_lib/mbed/api/
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/sw/lib/sw_lib/mbed/common/
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/sw/os
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/sw/os/os_dep/include
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/sw/os/freertos/include
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/sw/os/freertos/portable/GCC/ARM_CM3
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/targets/cmsis/
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/targets/cmsis/TARGET_RTK/TARGET_8195A
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/targets/hal/TARGET_RTK/TARGET_8195A
MODULE_IFLAGS += -I$(GCC_PROJ_BASE)/targets/hal/TARGET_RTK/TARGET_8195A/rtl8195a
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = 
OBJS_ROM = 
OBJS_RAM =
ifeq ($(CONFIG_TIMER_MODULE),y)
	OBJS += us_ticker_api.o wait_api.o
endif

ifeq ($(CONFIG_LIB_BUILD_RAM),y)
	OBJS = $(OBJS_RAM)
else ifeq ($(CONFIG_RELEASE_BUILD_RAM_ALL),y)
	OBJS += $(OBJS_RAM)
else ifeq ($(CONFIG_RELEASE_BUILD_LIBRARIES),y)
	OBJS = $(CSRC_ROM)
else ifeq ($(CONFIG_NORMAL_BUILD),y)
	OBJS += $(CSRC_ROM)
	OBJS += $(CSRC_RAM)
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


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
