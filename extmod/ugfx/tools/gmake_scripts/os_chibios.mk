#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

# See readme.txt for the make API

# Requirements:
#
# CHIBIOS_VERSION               Which version of ChibiOS is this (2 or 3) - default is 3
#


ifeq ($(CHIBIOS_VERSION),2)
  include $(GFXLIB)/tools/gmake_scripts/os_chibios_2.mk
else
  include $(GFXLIB)/tools/gmake_scripts/os_chibios_3.mk
endif
