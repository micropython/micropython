# Select the board to build for: if not given on the command line,
# then default to WIPY
BOARD ?= WIPY
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# Make 'release' the default build type
BTYPE ?= release

# If the build directory is not given, make it reflect the board name.
BUILD ?= build/$(BOARD)/$(BTYPE)

include ../py/mkenv.mk
-include ../../localconfig.mk

CROSS_COMPILE ?= arm-none-eabi-

CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -march=armv7e-m -mabi=aapcs -mcpu=cortex-m4 -msoft-float -mfloat-abi=soft -fsingle-precision-constant -Wdouble-promotion
CFLAGS = -Wall -Wpointer-arith -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) -Os
CFLAGS += -g -ffunction-sections -fdata-sections -fno-common -fsigned-char -mno-unaligned-access
CFLAGS += -Iboards/$(BOARD)

LDFLAGS = -Wl,-nostdlib -Wl,--gc-sections -Wl,-Map=$@.map

ifeq ($(BTARGET), application)
# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(BUILD)/pins_qstr.h
# include MicroPython make definitions
include ../py/py.mk
include application.mk
else
ifeq ($(BTARGET), bootloader)
include bootmgr/bootloader.mk
else
$(error Invalid BTARGET specified)
endif
endif

# always include MicroPython make rules
include ../py/mkrules.mk
