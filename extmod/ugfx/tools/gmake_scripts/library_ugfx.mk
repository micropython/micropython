#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

# See readme.txt for the make API

# Requirements:
#
# GFXLIB:		The location of the uGFX code eg GFXLIB=../ugfx
#

# Optional:
#
# GFXBOARD:			The uGFX Board to include eg GFXBOARD=Win32
# GFXDRIVERS:		The uGFX Drivers to include - separate multiple drivers with spaces eg GFXDRIVERS=multiple/uGFXnet
# GFXDEMO:			Compile a uGFX standard demo? If blank you need to include your own project files. eg GFXDEMO=modules/gwin/widgets
# GFXSINGLEMAKE:	Compile the entire system (except for board files and drivers) as a single file eg GFXSINGLEMAKE=yes (default is no)
#

PATHLIST += GFXLIB

include $(GFXLIB)/gfx.mk
SRC     += $(GFXSRC)
DEFS    += $(GFXDEFS)
INCPATH += $(GFXINC)
LIBS	+= $(GFXLIBS)
