# Possible Targets:	all clean Debug cleanDebug Release cleanRelease

##############################################################################################
# Settings
#

# General settings
	# See $(GFXLIB)/tools/gmake_scripts/readme.txt for the list of variables
	OPT_OS					= chibios
	OPT_THUMB				= yes
	OPT_LINK_OPTIMIZE		= yes
	OPT_CPU					= at91sam7

# uGFX settings
	# See $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk for the list of variables
	GFXLIB					= ../uGFX
	GFXBOARD				= Olimex-SAM7EX256-GE8
	GFXDEMO					= modules/gdisp/basics
	GFXSINGLEMAKE			= no

# ChibiOS settings
ifeq ($(OPT_OS),chibios)
	# See $(GFXLIB)/tools/gmake_scripts/os_chibios.mk for the list of variables
	CHIBIOS				= ../ChibiOS
	CHIBIOS_BOARD		= OLIMEX_SAM7_EX256
	CHIBIOS_PLATFORM	= AT91SAM7
	CHIBIOS_PORT		= GCC/ARM/AT91SAM7
	CHIBIOS_LDSCRIPT	= AT91SAM7X256.ld
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

# We override the standard ChibiOS linker script here just to give us some more stack space
LDSCRIPT = linker.ld

##############################################################################################
# These should be at the end
#

include $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk
include $(GFXLIB)/tools/gmake_scripts/os_$(OPT_OS).mk
include $(GFXLIB)/tools/gmake_scripts/compiler_gcc.mk
# *** EOF ***
