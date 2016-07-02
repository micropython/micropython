#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

#
# See readme.txt for the make API
#

# Requirements:
#
# NONE
#

SRCFLAGS += -mcpu=cortex-m3 -falign-functions=16
LDFLAGS  += -mcpu=cortex-m3
DEFS     += CORTEX_USE_FPU=FALSE

