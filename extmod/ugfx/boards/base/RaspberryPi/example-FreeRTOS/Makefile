# Possible Targets:	all clean Debug cleanDebug Release cleanRelease

##############################################################################################
# Settings
#

# General settings
	# See $(GFXLIB)/tools/gmake_scripts/readme.txt for the list of variables
	OPT_OS					= freertos
	OPT_THUMB				= yes
	OPT_LINK_OPTIMIZE		= yes
	# For the raspberry pi we can either use the generic armv6 cpu or the highly optimized raspberrypi settings
	OPT_CPU					= raspberrypi

# uGFX settings
	# See $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk for the list of variables
	GFXLIB					= ../uGFX
	GFXBOARD				= RaspberryPi
	GFXDEMO					= modules/gdisp/basics

# FreeRTOS settings
ifeq ($(OPT_OS),freertos)
	# See $(GFXLIB)/tools/gmake_scripts/os_freertos.mk for the list of variables
	FREERTOS			= ../FreeRTOS
	FREERTOS_BOARD		= RaspberryPi
	FREERTOS_MODULES	= $(GFXLIB)/boards/base/RaspberryPi/FreeRTOS
	FREERTOS_LDSCRIPT	= $(GFXLIB)/boards/base/RaspberryPi/FreeRTOS/raspberrypi.ld
endif

##############################################################################################
# Set these for your project
#

ARCH     = arm-none-eabi-
SRCFLAGS = -ggdb -O0
CFLAGS   =
CXXFLAGS = -fno-rtti
ASFLAGS  =
LDFLAGS  =

SRC      =
OBJS     =
DEFS     =
LIBS     =
INCPATH  =
LIBPATH  =

##############################################################################################
# These should be at the end
#

include $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk
include $(GFXLIB)/tools/gmake_scripts/os_$(OPT_OS).mk
include $(GFXLIB)/tools/gmake_scripts/compiler_gcc.mk
# *** EOF ***
