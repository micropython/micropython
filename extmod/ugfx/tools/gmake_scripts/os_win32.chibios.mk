#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

# See readme.txt for the make API

# Requirements:
#
# CHIBIOS:			The location of the ChibiOS code	eg CHIBIOS=../chibios
#

# Optional:
#

CHIBIOS_BOARD			= simulator
CHIBIOS_PLATFORM		= Win32
CHIBIOS_PORT			= GCC/SIMIA32

DEFS += SIMULATOR SHELL_USE_IPRINTF=FALSE

include $(GFXLIB)/tools/gmake_scripts/os_chibios.mk
include $(GFXLIB)/tools/gmake_scripts/os_win32.mk
