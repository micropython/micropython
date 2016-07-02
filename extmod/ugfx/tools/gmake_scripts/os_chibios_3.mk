#
# This file is subject to the terms of the GFX License. If a copy of
# the license was not distributed with this file, you can obtain one at:
#
#             http://ugfx.org/license.html
#

# See readme.txt for the make API

# Requirements:
#
# CHIBIOS                         The location of the ChibiOS code.    eg CHIBIOS = ../chibios
# CHIBIOS_CPUCLASS                The class of the CPU.    eg CHIBIOS_CPUCLASS = ARMCMx
# CHIBIOS_PLATFORM                The name of the ChibiOS platform.    eg CHIBIOS_PLATFORM = STM32
# CHIBIOS_DEVICE_FAMILY           The name of the ChibiOS device family.    eg CHIBIOS_DEVICE_FAMILY = STM32F4xx
# CHIBIOS_STARTUP                 The name of the ChibiOS startup files.    eg CHIBIOS_STARTUP = startup_stm32f4xx
# CHIBIOS_PORT                    The name of the ChibiOS port.    eg CHIBIOS_PORT = v7m
#

# Optional:
#
# CHIBIOS_LDSCRIPT                The name of the loader script    eg CHIBIOS_LDSCRIPT = STM32F429xI.ld
# CHIBIOS_BOARD                   The name of the ChibiOS board    eg CHIBIOS_BOARD = ST_STM32F429I_DISCOVERY - if not specified you must include equivalent code yourself
# CHIBIOS_VERSION                 Which version of ChibiOS is this (2 or 3) - default is 3
# CHIBIOS_PROCESS_STACKSIZE       Size of the ChibiOS process stack. Only useful if the link script supports it - default is 0x400
# CHIBIOS_EXCEPTIONS_STACKSIZE    Size of the ChibiOS exceptopms stack. Only useful if the link script supports it - default is 0x400
#

PATHLIST += CHIBIOS



include $(CHIBIOS)/os/common/ports/$(CHIBIOS_CPUCLASS)/compilers/GCC/mk/$(CHIBIOS_STARTUP).mk
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/$(CHIBIOS_PLATFORM)/$(CHIBIOS_DEVICE_FAMILY)/platform.mk
include $(CHIBIOS)/os/hal/osal/rt/osal.mk
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/rt/ports/$(CHIBIOS_CPUCLASS)/compilers/GCC/mk/port_$(CHIBIOS_PORT).mk
ifneq ($(CHIBIOS_BOARD),)
  include $(CHIBIOS)/os/hal/boards/$(CHIBIOS_BOARD)/board.mk
endif
LIBPATH += $(CHIBIOS)/os/common/ports/$(CHIBIOS_CPUCLASS)/compilers/GCC

ifeq ($(LDSCRIPT),)
  ifneq ($(CHIBIOS_LDSCRIPT),)
    LDSCRIPT= $(STARTUPLD)/$(CHIBIOS_LDSCRIPT)
  endif
endif

ifneq ($(LDSCRIPT),)
  ifeq ($(CHIBIOS_PROCESS_STACKSIZE),)
    LDFLAGS += -Wl,--defsym=__process_stack_size__=0x400
  else
    LDFLAGS += -Wl,--defsym=__process_stack_size__=$(CHIBIOS_PROCESS_STACKSIZE)
  endif
  ifeq ($(CHIBIOS_EXCEPTIONS_STACKSIZE),)
    LDFLAGS += -Wl,--defsym=__main_stack_size__=0x400
  else
    LDFLAGS += -Wl,--defsym=__main_stack_size__=$(CHIBIOS_EXCEPTIONS_STACKSIZE)
  endif
endif

# Add include files
INCPATH +=  $(STARTUPINC) \
            $(KERNINC) \
            $(PORTINC) \
            $(OSALINC) \
            $(HALINC) \
            $(PLATFORMINC) \
            $(BOARDINC)

# Add C files
SRC  += $(STARTUPSRC) \
        $(KERNSRC) \
        $(PORTSRC) \
        $(OSALSRC) \
        $(HALSRC) \
        $(PLATFORMSRC) \
        $(BOARDSRC)

# Add ASM files
SRC  += $(STARTUPASM) \
        $(PORTASM) \
        $(OSALASM)
