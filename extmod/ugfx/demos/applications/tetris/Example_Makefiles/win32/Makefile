# Possible Targets:	all clean Debug cleanDebug Release cleanRelease

##############################################################################################
# Settings
#

# General settings
	# See $(GFXLIB)/tools/gmake_scripts/readme.txt for the list of variables
	# For Win32 this variable can be set to "win32" (native win32 api) or "win32.chibios" (ChibiOS simulator). 
	OPT_OS					= win32
	OPT_LINK_OPTIMIZE		= yes

# uGFX settings
	# See $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk for the list of variables
	GFXLIB					= ../ugfx
	GFXBOARD				= Win32
	GFXDEMO					= applications/tetris

# ChibiOS settings
ifeq ($(OPT_OS),win32.chibios)
	# See $(GFXLIB)/tools/gmake_scripts/os_win32.chibios.mk for the list of variables
	CHIBIOS					= ../chibios
endif

# Win32 settings
	# See $(GFXLIB)/tools/gmake_scripts/os_win32.mk for the list of variables

##############################################################################################
# Set these for your project
#

ARCH     = i686-pc-mingw32-
SRCFLAGS = -ggdb -O0
CFLAGS   =
CXXFLAGS =
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
