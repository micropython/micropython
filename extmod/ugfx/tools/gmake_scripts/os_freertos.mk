#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

# See readme.txt for the make API

# Requirements:
#
# FREERTOS:			The location of the FreeRTOS code	eg FREERTOS=../FreeRTOS
# FREERTOS_BOARD	The board name						eg FREERTOS_BOARD=RaspberryPi
#

# Optional:
#
# FREERTOS_MODULES	 A list of directories containing FreeRTOS source (eg drivers, startup etc) - default is ""
# FREERTOS_LDSCRIPT  The loader script - default is ""
#

PATHLIST += FREERTOS

FREERTOS_MODULES += $(FREERTOS)/Source/portable/GCC/$(FREERTOS_BOARD)
FREERTOS_MODULES += $(FREERTOS)/Source/portable/MemMang
FREERTOS_MODULES += $(FREERTOS)/Source

INCPATH += $(FREERTOS)/Source/portable/GCC/$(FREERTOS_BOARD)	\
           $(FREERTOS)/Source/include

SRC     += $(foreach sdir,$(FREERTOS_MODULES),$(wildcard $(sdir)/*.s))
SRC     += $(foreach sdir,$(FREERTOS_MODULES),$(wildcard $(sdir)/*.c))

ifeq ($(LDSCRIPT),)
  LDSCRIPT= $(FREERTOS_LDSCRIPT)
endif
