# Possible Targets:	all clean Debug cleanDebug Release cleanRelease

##############################################################################################
# Settings
#

# General settings
	# See $(GFXLIB)/tools/gmake_scripts/readme.txt for the list of variables
	OPT_OS					= ecos
	OPT_LINK_OPTIMIZE		= yes
	OPT_CPU					= x86

# uGFX settings
	# See $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk for the list of variables
	GFXLIB					= ../uGFX
	GFXBOARD				= eCos-Synthetic-Framebuffer
	GFXDEMO					= modules/gdisp/basics

# ECOS settings
ifeq ($(OPT_OS),ecos)
	# See $(GFXLIB)/tools/gmake_scripts/os_ecos.mk for the list of variables
	FREERTOS			= ../eCos
	ECOS_LDSCRIPT		= target.ld
endif

##############################################################################################
# Set these for your project
#

ARCH     = $(ECOS_COMMAND_PREFIX)
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
