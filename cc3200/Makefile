# Select the board to build for: if not given on the command line,
# then default to WIPY
BOARD ?= WIPY
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# Make 'release' the default build type
BTYPE ?= release

# Port for flashing firmware
PORT ?= /dev/ttyUSB1

# If the build directory is not given, make it reflect the board name.
BUILD ?= build/$(BOARD)/$(BTYPE)

include ../py/mkenv.mk
-include ../../localconfig.mk

CROSS_COMPILE ?= arm-none-eabi-

CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -march=armv7e-m -mabi=aapcs -mcpu=cortex-m4 -msoft-float -mfloat-abi=soft -fsingle-precision-constant -Wdouble-promotion
CFLAGS = -Wall -Wpointer-arith -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) -Os
CFLAGS += -g -ffunction-sections -fdata-sections -fno-common -fsigned-char -mno-unaligned-access
CFLAGS += -Iboards/$(BOARD)
CFLAGS += $(CFLAGS_MOD)

LDFLAGS = -Wl,-nostdlib -Wl,--gc-sections -Wl,-Map=$@.map

FLASH_SIZE_WIPY = 2M
FLASH_SIZE_LAUNCHXL = 1M

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

erase:
	cc3200tool -p $(PORT) format_flash --size $(FLASH_SIZE_$(BOARD))

deploy:
	cc3200tool -p $(PORT) \
	    write_file bootmgr/build/$(BOARD)/$(BTYPE)/bootloader.bin /sys/mcuimg.bin \
	    write_file build/$(BOARD)/$(BTYPE)/mcuimg.bin /sys/factimg.bin

# Files *.ucf and *ucf.signed.bin come from CC3200SDK-SERVICEPACK
# package from http://www.ti.com/tool/cc3200sdk
servicepack:
	cc3200tool -p $(PORT) \
	write_file --file-size=0x20000 --signature ota_1.0.1.6-2.7.0.0.ucf.signed.bin \
	    ota_1.0.1.6-2.7.0.0.ucf /sys/servicepack.ucf
