#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

# See readme.txt for the make API

# Requirements:
#
# ECOS:  			The location of the eCos code		eg ECOS=../eCos
#

# Optional:
#
# ECOS_LDSCRIPT		The loader script - default is ""
#

PATHLIST += ECOS

# Not sure if this variable is needed by the ecos make.
INSTALL_DIR=$(ECOS)

include $(ECOS)/include/pkgconf/ecos.mak

INCPATH		+= $(ECOS)/include
LIBPATH		+= $(ECOS)/lib
LDFLAGS		+= -nostdlib

ifeq ($(LDSCRIPT),)
  LDSCRIPT= $(ECOS_LDSCRIPT)
endif
