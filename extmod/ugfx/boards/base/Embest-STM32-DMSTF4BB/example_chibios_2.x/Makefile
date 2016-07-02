# Possible Targets:	all clean Debug cleanDebug Release cleanRelease

##############################################################################################
# Settings
#

# General settings
	# See $(GFXLIB)/tools/gmake_scripts/readme.txt for the list of variables
	OPT_OS					= chibios
	OPT_THUMB				= yes
	OPT_LINK_OPTIMIZE		= no
	OPT_CPU					= stm32m4

# uGFX settings
	# See $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk for the list of variables
	GFXLIB					= ../path/to/ugfx
	GFXBOARD				= Embest-STM32-DMSTF4BB
	GFXDEMO					= modules/gdisp/basics

# ChibiOS settings
ifeq ($(OPT_OS),chibios)
	# See $(GFXLIB)/tools/gmake_scripts/os_chibios_x.mk for the list of variables
	CHIBIOS				= ../path/to/chibios
	CHIBIOS_VERSION		= 2
	CHIBIOS_BOARD		=
	CHIBIOS_PLATFORM	= STM32F4xx
	CHIBIOS_PORT		= GCC/ARMCMx/STM32F4xx
	CHIBIOS_LDSCRIPT	= STM32F407xG.ld
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
LDSCRIPT =

##############################################################################################
# These should be at the end
#

include $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk
include $(GFXLIB)/tools/gmake_scripts/os_$(OPT_OS).mk
include $(GFXLIB)/tools/gmake_scripts/compiler_gcc.mk
# *** EOF ***
