################################################################################
# Initial setup of Makefile environment

BOARD ?= ALIF_ENSEMBLE
BOARD_DIR ?= boards/$(BOARD)
BUILD ?= build-$(BOARD)

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ../../py/mkenv.mk
include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS += qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

################################################################################
# Project specific settings and compiler/linker flags

CROSS_COMPILE ?= arm-none-eabi-
GIT_SUBMODULES += lib/tinyusb lib/alif_ensemble-cmsis-dfp lib/alif-security-toolkit
PORT ?= /dev/ttyACM0

ALIF_TOOLS ?= ../../lib/alif-security-toolkit/toolkit
ALIF_DFP_REL_TOP ?= lib/alif_ensemble-cmsis-dfp
ALIF_DFP_REL_HERE ?= $(TOP)/lib/alif_ensemble-cmsis-dfp
CMSIS_DIR ?= $(TOP)/lib/cmsis/inc

MCU_CORE ?= M55_HP
ALIF_CONFIG ?= mcu/$(MCU_CORE)_cfg.json
LD_FILE ?= mcu/ensemble.ld.S

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(BOARD_DIR)
INC += -I$(CMSIS_DIR)
INC += -I$(ALIF_DFP_REL_HERE)/drivers/include/
INC += -I$(ALIF_DFP_REL_HERE)/ospi_xip/source/ospi
INC += -I$(ALIF_DFP_REL_HERE)/Device/common/config/
INC += -I$(ALIF_DFP_REL_HERE)/Device/common/include/
INC += -I$(ALIF_DFP_REL_HERE)/Device/core/$(MCU_CORE)/config/
INC += -I$(ALIF_DFP_REL_HERE)/Device/core/$(MCU_CORE)/include/
INC += -I$(ALIF_DFP_REL_HERE)/Device/$(MCU_SERIES)/$(MCU_VARIANT)/
INC += -I$(TOP)/lib/tinyusb/src
INC += -Itinyusb_port

GEN_PIN_MKPINS = mcu/make-pins.py
GEN_PIN_PREFIX = mcu/pins_prefix.c
GEN_PINS_BOARD_CSV = $(BOARD_DIR)/pins.csv
GEN_PINS_SRC = $(BUILD)/pins_board.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins_board.h

CFLAGS_FPU += -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS_CORTEX_M55 += -mthumb -mcpu=cortex-m55 -mtune=cortex-m55 $(CFLAGS_FPU)

CFLAGS += $(INC) -Wall -Werror -std=c99 $(CFLAGS_CORTEX_M55) -nostdlib
CFLAGS += -Wdouble-promotion -Wfloat-conversion
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -D$(MCU_CORE) -DCORE_$(MCU_CORE) -DALIF_CMSIS_H="\"$(MCU_CORE).h\""

ifeq ($(MICROPY_FLOAT_IMPL),float)
CFLAGS += -fsingle-precision-constant
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
else
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_DOUBLE
endif

AFLAGS = -mthumb -march=armv8.1-m.main $(CFLAGS_FPU)

LDFLAGS += -nostdlib
LDFLAGS += -T$(BUILD)/ensemble.ld -Map=$@.map --cref --gc-sections
LDFLAGS += --wrap=dcd_event_handler

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -Og -ggdb3
# Disable text compression in debug builds
MICROPY_ROM_TEXT_COMPRESSION = 0
else
CFLAGS += -O2 -DNDEBUG
endif

LIBS += "$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)"

JLINK_CMD = '\
ExitOnError 1\n\
Device $(JLINK_DEV)\n\
SelectInterface SWD\n\
Speed auto\n\
Connect\n\
Reset\n\
ShowHWStatus\n\
LoadFile "$(BUILD)/firmware_toc.bin",0x8057f1c0\n\
LoadFile "$(BUILD)/firmware.bin",0x80000000\n\
Reset\n\
Exit'

################################################################################
# Source files and libraries

SRC_O += \
	shared/runtime/gchelper_thumb2.o

SRC_C = \
	alif_flash.c \
	fatfs_port.c \
	machine_pin.c \
	main.c \
	modalif.c \
	mphalport.c \
	mpuart.c \
	msc_disk.c \
	ospi_flash.c \
	pendsv.c \
	usbd.c \
	$(wildcard $(BOARD_DIR)/*.c)

ifeq ($(MICROPY_FLOAT_IMPL),float)
LIBM_SRC_C += $(SRC_LIB_LIBM_C)
LIBM_SRC_C += $(SRC_LIB_LIBM_SQRT_HW_C)
$(BUILD)/lib/libm/%.o: CFLAGS += -Wno-maybe-uninitialized
else
LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_C)
LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_SQRT_HW_C)
$(BUILD)/lib/libm_dbl/%.o: CFLAGS += -Wno-maybe-uninitialized
endif

SHARED_SRC_C += $(addprefix shared/,\
	libc/string0.c \
	netutils/dhcpserver.c \
	netutils/netutils.c \
	netutils/trace.c \
	readline/readline.c \
	runtime/gchelper_native.c \
	runtime/interrupt_char.c \
	runtime/mpirq.c \
	runtime/pyexec.c \
	runtime/softtimer.c  \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	timeutils/timeutils.c \
	tinyusb/mp_usbd.c \
	tinyusb/mp_usbd_cdc.c \
	tinyusb/mp_usbd_descriptor.c \
	)

DRIVERS_SRC_C += $(addprefix drivers/,\
	bus/softspi.c \
	bus/softqspi.c \
	memory/spiflash.c \
	dht/dht.c \
	)

TINYUSB_SRC_C += \
	lib/tinyusb/src/tusb.c \
	lib/tinyusb/src/class/cdc/cdc_device.c \
	lib/tinyusb/src/class/msc/msc_device.c \
	lib/tinyusb/src/common/tusb_fifo.c \
	lib/tinyusb/src/device/usbd.c \
	lib/tinyusb/src/device/usbd_control.c \
	tinyusb_port/tusb_alif_dcd.c \

ALIF_SRC_C += $(addprefix $(ALIF_DFP_REL_TOP)/,\
	Device/common/source/clk.c \
	Device/common/source/mpu_M55.c \
	Device/common/source/system_M55.c \
	Device/common/source/system_utils.c \
	Device/core/$(MCU_CORE)/source/startup_$(MCU_CORE).c \
	drivers/source/pinconf.c \
	drivers/source/uart.c \
	ospi_xip/source/ospi/ospi_drv.c \
	)

$(BUILD)/tinyusb_port/tusb_alif_dcd.o: CFLAGS += -Wno-unused-variable -DTUSB_ALIF_NO_IRQ_CFG=1

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SHARED_SRC_C) $(GEN_PINS_SRC)

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(TINYUSB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(ALIF_SRC_C:.c=.o))
OBJ += $(GEN_PINS_SRC:.c=.o)

################################################################################
# Main targets

.DELETE_ON_ERROR:

.PHONY: all
all: $(BUILD)/firmware_toc.bin

$(BUILD)/ensemble.ld: $(LD_FILE)
	$(ECHO) "Preprocess linker script $@"
	$(Q)$(CPP) -P -E $(CFLAGS) $^ > $@

$(BUILD)/firmware.elf: $(OBJ) $(BUILD)/ensemble.ld
	$(ECHO) "Link $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -Obinary $^ $(BUILD)/firmware.bin

$(BUILD)/firmware_toc.bin: $(BUILD)/firmware.bin
	$(Q)python $(ALIF_TOOLS)/app-gen-toc.py \
		--filename $(abspath $(BUILD)/$(ALIF_TOC_CONFIG)) \
		--output-dir $(BUILD) \
		--firmware-dir $(BUILD) \
		--output $@

.PHONY: deploy
deploy: $(BUILD)/firmware_toc.bin
	$(ECHO) "Writing $< to the board"
	$(Q)python $(ALIF_TOOLS)/app-write-mram.py \
		--cfg-part $(ALIF_TOOLKIT_CFG_PART) \
		--port $(PORT) \
		--pad \
		--images file:$(BUILD)/application_package.ds

.PHONY: deploy-jlink
deploy-jlink: $(BUILD)/firmware_toc.bin
	$(Q)echo -e $(JLINK_CMD) | $(JLINK_EXE)

.PHONY: maintenance
maintenance:
	$(Q)python $(ALIF_TOOLS)/maintenance.py \
		--cfg-part $(ALIF_TOOLKIT_CFG_PART) \
		--port $(PORT)

.PHONY: update-system-package
update-system-package:
	$(Q)python $(ALIF_TOOLS)/updateSystemPackage.py \
		--cfg-part $(ALIF_TOOLKIT_CFG_PART) \
		--port $(PORT)

################################################################################
# Remaining make rules

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_board.c and pins_board.h
$(BUILD)/%_board.c $(HEADER_BUILD)/%_board.h: $(BOARD_DIR)/%.csv $(GEN_PIN_MKPINS) $(GEN_PIN_PREFIX) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(GEN_PIN_MKPINS) \
	    --board-csv $(GEN_PINS_BOARD_CSV) \
	    --prefix $(GEN_PIN_PREFIX) \
	    --output-source $(GEN_PINS_SRC) \
	    --output-header $(GEN_PINS_HDR)

include $(TOP)/py/mkrules.mk
