# Select the board to build for:
ifdef BOARD_DIR
# Custom board path - remove trailing slash and get the final component of
# the path as the board name.
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
# If not given on the command line, then default to RA6M2_EK.
BOARD ?= EK_RA6M2
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

# select use wrapper function of FSP library
USE_FSP_FLASH = 1

include ../../py/mkenv.mk
-include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk

USE_FSP_LPM ?= 1
USE_FSP_QSPI ?= 0
FSP_BOARD_NAME ?= $(shell echo $(BOARD) | tr '[:upper:]' '[:lower:]')

# qstr definitions (must come before including py.mk)
QSTR_DEFS += qstrdefsport.h

# Files that are generated and needed before the QSTR build.
QSTR_GLOBAL_DEPENDENCIES += mpconfigboard_common.h $(BOARD_DIR)/mpconfigboard.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# File containing description of content to be frozen into firmware.
FROZEN_MANIFEST ?= boards/manifest.py

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

GIT_SUBMODULES += lib/fsp lib/tinyusb

MCU_SERIES_UPPER = $(shell echo $(MCU_SERIES) | tr '[:lower:]' '[:upper:]')
CMSIS_MCU_LOWER = $(shell echo $(CMSIS_MCU) | tr '[:upper:]' '[:lower:]')

LD_DIR=boards
CMSIS_DIR=lib/cmsis/inc
HAL_DIR=lib/fsp
STARTUP_FILE ?= lib/fsp/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/startup.o
SYSTEM_FILE ?= lib/fsp/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/system.o

# Select the cross compile prefix
CROSS_COMPILE ?= arm-none-eabi-

INC += -imacros boards/compiler_barrier.h
INC += -I.
INC += -Ifsp_cfg
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TOP)/$(CMSIS_DIR)
INC += -I$(TOP)/$(HAL_DIR)
INC += -I$(TOP)/$(HAL_DIR)/ra/fsp/inc
INC += -I$(TOP)/$(HAL_DIR)/ra/fsp/inc/api
INC += -I$(TOP)/$(HAL_DIR)/ra/fsp/inc/instances
INC += -I$(TOP)/$(HAL_DIR)/ra/fsp/src/bsp/cmsis/Device/RENESAS/Include
INC += -I$(TOP)/lib/tinyusb/hw
INC += -I$(TOP)/lib/tinyusb/src
INC += -I$(TOP)/shared/tinyusb
INC += -Ilwip_inc
ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA4M1 RA4W1 RA6M1 RA6M2 RA6M5))
INC += -Ira
endif
INC += -I$(BOARD_DIR)/ra_gen
INC += -I$(BOARD_DIR)/ra_cfg/driver
INC += -I$(BOARD_DIR)/ra_cfg/fsp_cfg
INC += -I$(BOARD_DIR)/ra_cfg/fsp_cfg/bsp
INC += -Idebug

CFLAGS += -D$(CMSIS_MCU)
CFLAGS += -DRA_HAL_H='<$(CMSIS_MCU)_hal.h>'
CFLAGS += -DRA_CFG_H='<$(FSP_BOARD_NAME)_conf.h>'
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_RAXXX

# Basic Cortex-M flags
CFLAGS_CORTEX_M = -mthumb

# Select hardware floating-point support
SUPPORTS_HARDWARE_FP_SINGLE = 0
SUPPORTS_HARDWARE_FP_DOUBLE = 0
ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),m4))
CFLAGS_CORTEX_M += -mfpu=fpv4-sp-d16 -mfloat-abi=hard
SUPPORTS_HARDWARE_FP_SINGLE = 1
endif

ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),m33))
CFLAGS_CORTEX_M += -mfpu=fpv5-sp-d16 -mfloat-abi=hard
SUPPORTS_HARDWARE_FP_SINGLE = 1
endif

# Options for particular MCU series
CFLAGS_MCU_RA4M1 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_RA4W1 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_RA6M1 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_RA6M2 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_RA6M5 = $(CFLAGS_CORTEX_M) -mtune=cortex-m33 -mcpu=cortex-m33

ASFLAGS += $(CFLAGS_CORTEX_M) -mcpu=cortex-$(MCU_SERIES)
CFLAGS += $(INC) -Wall -Wpointer-arith -Werror -Wdouble-promotion -Wfloat-conversion -std=gnu99 -nostdlib $(CFLAGS_EXTRA)
#CFLAGS += -D$(CMSIS_MCU)
CFLAGS += $(CFLAGS_MCU_$(CMSIS_MCU))
CFLAGS += $(COPT)
CFLAGS += -I$(BOARD_DIR)

# Configure floating point support
ifeq ($(MICROPY_FLOAT_IMPL),double)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_DOUBLE
else
ifeq ($(MICROPY_FLOAT_IMPL),none)
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_NONE
else
CFLAGS += -DMICROPY_FLOAT_IMPL=MICROPY_FLOAT_IMPL_FLOAT
CFLAGS += -fsingle-precision-constant
endif
endif

LDFLAGS += -nostdlib -L $(LD_DIR) $(addprefix -T,$(LD_FILES)) -Map=$(@:.elf=.map) --cref
LDFLAGS += --defsym=_estack_reserve=8
LIBS += "$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)"

# Remove uncalled code from the final image.
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += --gc-sections

# Debugging/Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -DPENDSV_DEBUG
#COPT = -Og
COPT = -Os
# Disable text compression in debug builds
MICROPY_ROM_TEXT_COMPRESSION = 0
else
COPT += -Os -DNDEBUG
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -Wmissing-prototypes -Wold-style-definition -std=gnu99,$(CFLAGS))

# TODO make this common -- shouldn't be using these "private" vars from py.mk
ifneq ($(SRC_CXX)$(SRC_USERMOD_CXX)$(SRC_USERMOD_LIB_CXX),)
LIBSTDCPP_FILE_NAME = "$(shell $(CXX) $(CXXFLAGS) -print-file-name=libstdc++.a)"
LDFLAGS += -L"$(shell dirname $(LIBSTDCPP_FILE_NAME))"
endif

# Options for mpy-cross
MPY_CROSS_FLAGS += -march=armv7m

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
	runtime/softtimer.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	timeutils/timeutils.c \
	tinyusb/mp_usbd.c \
	tinyusb/mp_usbd_cdc.c \
	tinyusb/mp_usbd_descriptor.c \
	)

# TinyUSB Stack source
TINYUSB_SRC_C += $(addprefix lib/tinyusb/,\
	src/class/cdc/cdc_device.c \
	src/class/dfu/dfu_rt_device.c \
	src/class/hid/hid_device.c \
	src/class/midi/midi_device.c \
	src/class/msc/msc_device.c \
	src/class/usbtmc/usbtmc_device.c \
	src/class/vendor/vendor_device.c \
	src/common/tusb_fifo.c \
	src/device/usbd.c \
	src/device/usbd_control.c \
	src/portable/renesas/rusb2/dcd_rusb2.c \
	src/portable/renesas/rusb2/hcd_rusb2.c \
	src/portable/renesas/rusb2/rusb2_common.c \
	src/tusb.c \
	)

ifeq ($(MICROPY_FLOAT_IMPL),double)
LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_C)
ifeq ($(SUPPORTS_HARDWARE_FP_DOUBLE),1)
LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_SQRT_HW_C)
else
LIBM_SRC_C += $(SRC_LIB_LIBM_DBL_SQRT_SW_C)
endif
else
LIBM_SRC_C += $(SRC_LIB_LIBM_C)
ifeq ($(SUPPORTS_HARDWARE_FP_SINGLE),1)
LIBM_SRC_C += $(SRC_LIB_LIBM_SQRT_HW_C)
else
LIBM_SRC_C += $(SRC_LIB_LIBM_SQRT_SW_C)
endif
endif

DRIVERS_SRC_C += $(addprefix drivers/,\
	bus/softspi.c \
	bus/softqspi.c \
	memory/spiflash.c \
	dht/dht.c \
	)

SRC_C += \
	boardctrl.c \
	main.c \
	ra_hal.c \
	ra_it.c \
	rng.c \
	mphalport.c \
	mpnetworkport.c \
	mpthreadport.c \
	irq.c \
	pendsv.c \
	systick.c  \
	powerctrl.c \
	powerctrlboot.c \
	pybthread.c \
	factoryreset.c \
	timer.c \
	led.c \
	uart.c \
	gccollect.c \
	help.c \
	machine_dac.c \
	machine_i2c.c \
	machine_spi.c \
	machine_pin.c \
	machine_rtc.c \
	machine_sdcard.c \
	modrenesas.c \
	extint.c \
	usrsw.c \
	flash.c \
	flashbdev.c \
	storage.c \
	fatfs_port.c \
	usbd.c \
	$(wildcard $(BOARD_DIR)/*.c)

SRC_C += $(addprefix $(BOARD_DIR)/ra_gen/,\
	common_data.c \
	hal_data.c \
	pin_data.c \
	vector_data.c \
	)

ifeq ($(MICROPY_SSL_MBEDTLS),1)
LIB_SRC_C += mbedtls/mbedtls_port.c
endif

ifeq ($(MICROPY_PY_BLUETOOTH),1)
SRC_C += mpbthciport.c
endif

ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)
SRC_C += mpnimbleport.c
endif

SRC_O += \
	$(STARTUP_FILE) \
	$(SYSTEM_FILE)

SRC_O += \
	shared/runtime/gchelper_thumb2.o

HAL_SRC_C += $(addprefix $(HAL_DIR)/ra/fsp/src/bsp/mcu/all/,\
	bsp_clocks.c \
	bsp_common.c \
	bsp_delay.c \
	bsp_group_irq.c \
	bsp_guard.c \
	bsp_io.c \
	bsp_irq.c \
	bsp_register_protection.c \
	bsp_rom_registers.c \
	bsp_sbrk.c \
	bsp_security.c \
	)

HAL_SRC_C += $(addprefix $(HAL_DIR)/ra/fsp/src/,\
	r_ioport/r_ioport.c \
	r_sci_uart/r_sci_uart.c \
	)

CFLAGS_FSP = -Wno-unused-variable -Wno-unused-function
$(BUILD)/lib/fsp/ra/fsp/src/r_sci_uart/r_sci_uart.o: CFLAGS += $(CFLAGS_FSP)

ifeq ($(USE_FSP_QSPI), 1)
CFLAGS += -DUSE_FSP_QSPI
HAL_SRC_C += $(HAL_DIR)/ra/fsp/src/r_qspi/r_qspi.c
endif

ifeq ($(USE_FSP_SDHI), 1)
CFLAGS += -DUSE_FSP_SDHI
HAL_SRC_C += $(HAL_DIR)/ra/fsp/src/r_sdhi/r_sdhi.c \
	$(HAL_DIR)/ra/fsp/src/r_dtc/r_dtc.c
endif

ifeq ($(USE_FSP_LPM), 1)
CFLAGS += -DUSE_FSP_LPM
HAL_SRC_C += $(HAL_DIR)/ra/fsp/src/r_lpm/r_lpm.c
endif

ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA4M1 RA4W1))
ifeq ($(USE_FSP_FLASH), 1)
CFLAGS += -DUSE_FSP_FLASH
HAL_SRC_C += $(HAL_DIR)/ra/fsp/src/r_flash_lp/r_flash_lp.c
endif
endif

ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA6M1 RA6M2 RA6M5))
ifeq ($(USE_FSP_FLASH), 1)
CFLAGS += -DUSE_FSP_FLASH
HAL_SRC_C += $(HAL_DIR)/ra/fsp/src/r_flash_hp/r_flash_hp.c
endif
endif

ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA4M1 RA4W1 RA6M1 RA6M2 RA6M5))
HAL_SRC_C += $(addprefix ra/,\
	ra_adc.c \
	ra_dac.c \
	ra_flash.c \
	ra_gpio.c \
	ra_i2c.c \
	ra_icu.c \
	ra_init.c \
	ra_int.c \
	ra_rtc.c \
	ra_sci.c \
	ra_spi.c \
	ra_timer.c \
	ra_gpt.c \
	ra_utils.c \
	)

endif

ifeq ($(MICROPY_HW_ENABLE_RNG),1)
CRYPTO_DIR = ra/fsp/src/r_sce_protected/crypto_procedures_protected/src/sce9/

INC += -I$(TOP)/$(HAL_DIR)/$(CRYPTO_DIR)/inc
INC += -I$(TOP)/$(HAL_DIR)/$(CRYPTO_DIR)/inc/api
INC += -I$(TOP)/$(HAL_DIR)/$(CRYPTO_DIR)/private/inc
INC += -I$(TOP)/$(HAL_DIR)/$(CRYPTO_DIR)/inc/instances

# The following FSP files are required for the random number generator.
HAL_SRC_C += $(addprefix $(HAL_DIR)/$(CRYPTO_DIR)/,\
	public/r_sce.c \
	public/r_sce_ecc.c \
	public/r_sce_sha.c \
	public/r_sce_aes.c \
	private/r_sce_private.c \
	primitive/r_sce_p00.c \
	primitive/r_sce_p20.c \
	primitive/r_sce_p26.c \
	primitive/r_sce_p81.c \
	primitive/r_sce_p82.c \
	primitive/r_sce_p92.c \
	primitive/r_sce_p40.c \
	primitive/r_sce_func050.c \
	primitive/r_sce_func051.c \
	primitive/r_sce_func052.c \
	primitive/r_sce_func053.c \
	primitive/r_sce_func054.c \
	primitive/r_sce_func100.c \
	primitive/r_sce_func101.c \
	primitive/r_sce_func040.c \
	primitive/r_sce_func048.c \
	primitive/r_sce_func102.c \
	primitive/r_sce_func103.c \
	primitive/r_sce_subprc01.c\
	)
endif

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(HAL_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(TINYUSB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(BUILD)/pins_$(BOARD).o

ifeq ($(CMSIS_MCU),RA6M5)
$(BUILD)/shared/runtime/gchelper_thumb2.o: $(TOP)/shared/runtime/gchelper_thumb2.s
	$(ECHO) "AS $<"
	$(Q)$(AS) $(ASFLAGS) -o $@ $<
endif

# This file contains performance critical functions so turn up the optimisation
# level.  It doesn't add much to the code size and improves performance a bit.
# Don't use -O3 with this file because gcc tries to optimise memset in terms of itself.
$(BUILD)/shared/libc/string0.o: COPT += -O2

# We put several files into the first 16K section with the ISRs.
# If we compile these using -O0 then it won't fit. So if you really want these
# to be compiled with -O0, then edit boards/common.ld (in the .isr_vector section)
# and comment out the following lines.
$(BUILD)/$(OOFATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os
$(PY_BUILD)/formatfloat.o: COPT += -Os
$(PY_BUILD)/parsenum.o: COPT += -Os
$(PY_BUILD)/mpprint.o: COPT += -Os

all: $(TOP)/lib/fsp/README.md $(BUILD)/firmware.hex $(BUILD)/firmware.bin

# For convenience, automatically fetch required submodules if they don't exist
$(TOP)/lib/fsp/README.md:
	$(ECHO) "fsp submodule not found, fetching it now..."
	(cd $(TOP) && git submodule update --init lib/fsp)

define GENERATE_ELF
	$(ECHO) "LINK $(1)"
	$(Q)$(LD) $(LDFLAGS) -o $(1) $(2) $(LDFLAGS_MOD) $(LIBS)
	$(Q)$(SIZE) $(1)
endef

define GENERATE_BIN
	$(ECHO) "GEN $(1)"
	$(Q)$(OBJCOPY) -O binary -j .id_code $(2) $(BUILD)/id_code.bin
	$(Q)$(OBJCOPY) -O binary --remove-section=.id_code $(2) $(1)
endef

define GENERATE_HEX
	$(ECHO) "GEN $(1)"
	$(Q)$(OBJCOPY) -O ihex $(2) $(1)
endef

.PHONY:

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(call GENERATE_BIN,$@,$^)

$(BUILD)/firmware.hex: $(BUILD)/firmware.elf
	$(call GENERATE_HEX,$@,$^)

$(BUILD)/firmware.elf: $(OBJ)
	$(call GENERATE_ELF,$@,$^)

MAKE_PINS = boards/make-pins.py
BOARD_PINS = $(BOARD_DIR)/pins.csv
PREFIX_FILE = boards/ra_pin_prefix.c
ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA4M1))
AF_FILE = boards/ra4m1_af.csv
endif
ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA4W1))
AF_FILE = boards/ra4w1_af.csv
endif
ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA6M1))
AF_FILE = boards/ra6m1_af.csv
endif
ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA6M2))
AF_FILE = boards/ra6m2_af.csv
endif
ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA6M5))
AF_FILE = boards/ra6m5_af.csv
endif
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h

FILE2H = $(TOP)/tools/file2h.py

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_CXX) $(SHARED_SRC_C) $(GEN_PINS_SRC)

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(GEN_PINS_HDR)

# With conditional pins, we may need to regenerate qstrdefs.h when config
# options change.
$(HEADER_BUILD)/qstrdefs.generated.h: $(BOARD_DIR)/mpconfigboard.h

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
.PRECIOUS: $(GEN_PINS_SRC)
$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h: $(BOARD_DIR)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board-csv $(BOARD_PINS) --af-csv $(AF_FILE) --prefix $(PREFIX_FILE) \
	    --output-source $(GEN_PINS_SRC) --output-header $(GEN_PINS_HDR)

CMSIS_MCU_HDR = $(CMSIS_DIR)/$(CMSIS_MCU_LOWER).h

include $(TOP)/py/mkrules.mk
