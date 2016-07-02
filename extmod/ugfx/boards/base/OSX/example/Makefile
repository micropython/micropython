# Possible Targets:	all clean Debug cleanDebug Release cleanRelease

##############################################################################################
# Settings
#

# General settings
	# See $(GFXLIB)/tools/gmake_scripts/readme.txt for the list of variables
	OPT_OS					= osx
	OPT_LINK_OPTIMIZE		= no

# uGFX settings
	# See $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk for the list of variables
	GFXLIB					= ../../path/to/ugfx
	GFXBOARD				= OSX
	GFXDEMO					= modules/gdisp/basics

# OSX settings
	# See $(GFXLIB)/tools/gmake_scripts/os_osx.mk for the list of variables
	OSX_SDK = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk
	OSX_ARCH = -mmacosx-version-min=10.11

##############################################################################################
# Set these for your project
#

ARCH     =
SRCFLAGS = -ggdb -O0
CFLAGS   =
CXXFLAGS =
ASFLAGS  =
LDFLAGS  =

SRC      =
OBJS     =
DEFS     =
LIBS     =
INCPATH  = /opt/X11/include
LIBPATH  = /opt/X11/lib

##############################################################################################
# These should be at the end
#

include $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk
include $(GFXLIB)/tools/gmake_scripts/os_$(OPT_OS).mk
include $(GFXLIB)/tools/gmake_scripts/compiler_gcc.mk
# *** EOF ***
