# Select the board to build for: if not given on the command line,
# then default to RA6M2_EK.
BOARD ?= RA6M2_EK

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

BOARD_DIR ?= boards/$(BOARD)
ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

ifeq ($(BOARD),RA4M1_CLICKER)
BOARD_LOW = ra4m1_ek
CMSIS_MCU_LOW = ra4m1
CMSIS_MCU_CAP = RA4M1
USE_FSP_LPM = 0
endif

ifeq ($(BOARD),RA4M1_EK)
BOARD_LOW = ra4m1_ek
CMSIS_MCU_LOW = ra4m1
CMSIS_MCU_CAP = RA4M1
USE_FSP_LPM = 0
endif

ifeq ($(BOARD),RA4W1_EK)
BOARD_LOW = ra4w1_ek
CMSIS_MCU_LOW = ra4w1
CMSIS_MCU_CAP = RA4W1
USE_FSP_LPM = 1
endif

ifeq ($(BOARD),RA6M1_EK)
BOARD_LOW = ra6m1_ek
CMSIS_MCU_LOW = ra6m1
CMSIS_MCU_CAP = RA6M1
USE_FSP_LPM = 1
endif

ifeq ($(BOARD),RA6M2_EK)
BOARD_LOW = ra6m2_ek
CMSIS_MCU_LOW = ra6m2
CMSIS_MCU_CAP = RA6M2
USE_FSP_LPM = 1
endif

# select use wrapper function of FSP library
USE_FSP_FLASH = 1

include ../../py/mkenv.mk
-include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk

# Files that are generated and needed before the QSTR build.
#QSTR_GENERATED_HEADERS = $(BUILD)/pins_qstr.h $(BUILD)/modstm_qstr.h
QSTR_GENERATED_HEADERS = $(BUILD)/pins_qstr.h
# qstr definitions (must come before including py.mk)
QSTR_DEFS += qstrdefsport.h $(QSTR_GENERATED_HEADERS)
QSTR_GLOBAL_DEPENDENCIES += mpconfigboard_common.h $(BOARD_DIR)/mpconfigboard.h $(QSTR_GENERATED_HEADERS)

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# File containing description of content to be frozen into firmware.
FROZEN_MANIFEST ?= boards/manifest.py

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

GIT_SUBMODULES += lib/fsp

MCU_SERIES_UPPER = $(shell echo $(MCU_SERIES) | tr '[:lower:]' '[:upper:]')
CMSIS_MCU_LOWER = $(shell echo $(CMSIS_MCU) | tr '[:upper:]' '[:lower:]')

LD_DIR=boards
CMSIS_DIR=lib/cmsis/inc
HAL_DIR=lib/fsp
STARTUP_FILE ?= lib/fsp/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/startup.o
SYSTEM_FILE ?= lib/fsp/ra/fsp/src/bsp/cmsis/Device/RENESAS/Source/system.o

# Select the cross compile prefix
CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(TOP)/$(CMSIS_DIR)
INC += -I$(TOP)/$(HAL_DIR)
INC += -I$(TOP)/$(HAL_DIR)/ra/fsp/inc
INC += -I$(TOP)/$(HAL_DIR)/ra/fsp/inc/api
INC += -I$(TOP)/$(HAL_DIR)/ra/fsp/inc/instances
INC += -I$(TOP)/$(HAL_DIR)/ra/fsp/src/bsp/cmsis/Device/RENESAS/Include
#INC += -Ilwip_inc
ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA4M1 RA4W1 RA6M1 RA6M2))
INC += -Ira
endif
INC += -I$(BOARD_DIR)/ra_gen
INC += -I$(BOARD_DIR)/ra_cfg/fsp_cfg
INC += -I$(BOARD_DIR)/ra_cfg/fsp_cfg/bsp
INC += -Idebug

CFLAGS += -D$(CMSIS_MCU)
CFLAGS += -DRA_HAL_H='<$(CMSIS_MCU)_hal.h>'

# Basic Cortex-M flags
CFLAGS_CORTEX_M = -mthumb

# Select hardware floating-point support
SUPPORTS_HARDWARE_FP_SINGLE = 0
SUPPORTS_HARDWARE_FP_DOUBLE = 0
ifeq ($(MCU_SERIES),$(filter $(MCU_SERIES),m4))
CFLAGS_CORTEX_M += -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
SUPPORTS_HARDWARE_FP_SINGLE = 1
endif

# Options for particular MCU series
CFLAGS_MCU_RA4M1 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_RA4W1 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_RA6M1 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4
CFLAGS_MCU_RA6M2 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4

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
	)

ifeq ($(MICROPY_FLOAT_IMPL),double)
LIBM_SRC_C += $(addprefix lib/libm_dbl/,\
	__cos.c \
	__expo2.c \
	__fpclassify.c \
	__rem_pio2.c \
	__rem_pio2_large.c \
	__signbit.c \
	__sin.c \
	__tan.c \
	acos.c \
	acosh.c \
	asin.c \
	asinh.c \
	atan.c \
	atan2.c \
	atanh.c \
	ceil.c \
	cos.c \
	cosh.c \
	copysign.c \
	erf.c \
	exp.c \
	expm1.c \
	floor.c \
	fmod.c \
	frexp.c \
	ldexp.c \
	lgamma.c \
	log.c \
	log10.c \
	log1p.c \
	modf.c \
	nearbyint.c \
	pow.c \
	rint.c \
	round.c \
	scalbn.c \
	sin.c \
	sinh.c \
	tan.c \
	tanh.c \
	tgamma.c \
	trunc.c \
	)
ifeq ($(SUPPORTS_HARDWARE_FP_DOUBLE),1)
LIBM_SRC_C += lib/libm_dbl/thumb_vfp_sqrt.c
else
LIBM_SRC_C += lib/libm_dbl/sqrt.c
endif
else
LIBM_SRC_C += $(addprefix lib/libm/,\
	math.c \
	acoshf.c \
	asinfacosf.c \
	asinhf.c \
	atan2f.c \
	atanf.c \
	atanhf.c \
	ef_rem_pio2.c \
	erf_lgamma.c \
	fmodf.c \
	kf_cos.c \
	kf_rem_pio2.c \
	kf_sin.c \
	kf_tan.c \
	log1pf.c \
	nearbyintf.c \
	roundf.c \
	sf_cos.c \
	sf_erf.c \
	sf_frexp.c \
	sf_ldexp.c \
	sf_modf.c \
	sf_sin.c \
	sf_tan.c \
	wf_lgamma.c \
	wf_tgamma.c \
	)
ifeq ($(SUPPORTS_HARDWARE_FP_SINGLE),1)
LIBM_SRC_C += lib/libm/thumb_vfp_sqrtf.c
else
LIBM_SRC_C += lib/libm/ef_sqrt.c
endif
endif

LIBM_O = $(addprefix $(BUILD)/, $(LIBM_SRC_C:.c=.o))

# Too many warnings in libm_dbl, disable for now.
ifeq ($(MICROPY_FLOAT_IMPL),double)
$(LIBM_O): CFLAGS := $(filter-out -Wdouble-promotion -Wfloat-conversion, $(CFLAGS))
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
	ra_it.c \
	mphalport.c \
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
	machine_adc.c \
	machine_i2c.c \
	machine_spi.c \
	machine_uart.c \
	machine_pin.c \
	machine_rtc.c \
	modmachine.c \
	modutime.c \
	extint.c \
	usrsw.c \
	flash.c \
	flashbdev.c \
	storage.c \
	fatfs_port.c \
	$(BOARD_DIR)/src/hal_entry.c \
	$(wildcard $(BOARD_DIR)/*.c)

SRC_C += $(addprefix $(BOARD_DIR)/ra_gen/,\
	common_data.c \
	hal_data.c \
	main.c \
	pin_data.c \
	vector_data.c \
	)

SRC_O += \
	$(STARTUP_FILE) \
	$(SYSTEM_FILE)

SRC_O += \
	shared/runtime/gchelper_thumb2.o

HAL_SRC_C += $(addprefix $(HAL_DIR)/ra/board/$(BOARD_LOW)/,\
	board_init.c \
	board_leds.c \
	)

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
	)

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

ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA6M1 RA6M2))
ifeq ($(USE_FSP_FLASH), 1)
CFLAGS += -DUSE_FSP_FLASH
HAL_SRC_C += $(HAL_DIR)/ra/fsp/src/r_flash_hp/r_flash_hp.c
endif
endif

ifeq ($(CMSIS_MCU),$(filter $(CMSIS_MCU),RA4M1 RA4W1 RA6M1 RA6M2))
HAL_SRC_C += $(addprefix ra/,\
	ra_adc.c \
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
	ra_utils.c \
	)

endif

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(LIBM_O)
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(HAL_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_O))
OBJ += $(BUILD)/pins_$(BOARD).o

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

all: $(TOP)/lib/fsp/README.md $(BUILD)/firmware.hex

# For convenience, automatically fetch required submodules if they don't exist
$(TOP)/lib/fsp/README.md:
	$(ECHO) "fsp submodule not found, fetching it now..."
	(cd $(TOP) && git submodule update --init lib/fsp)

ifneq ($(FROZEN_MANIFEST)$(FROZEN_DIR),)
# To use frozen source modules, put your .py files in a subdirectory (eg scripts/)
# and then invoke make with FROZEN_DIR=scripts (be sure to build from scratch).
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
endif

ifneq ($(FROZEN_MANIFEST)$(FROZEN_MPY_DIR),)
# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen (be sure to build from scratch).
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
endif

define GENERATE_ELF
	$(ECHO) "LINK $(1)"
	$(Q)$(LD) $(LDFLAGS) -o $(1) $(2) $(LDFLAGS_MOD) $(LIBS)
	$(Q)$(SIZE) $(1)
endef

define GENERATE_BIN
	$(ECHO) "GEN $(1)"
	$(Q)$(OBJCOPY) -O binary $(addprefix -j ,$(3)) $(2) $(1)
endef

define GENERATE_HEX
	$(ECHO) "GEN $(1)"
	$(Q)$(OBJCOPY) -O ihex $(2) $(1)
endef

.PHONY: 

# A board should specify TEXT0_ADDR if to use a different location than the
# default for the firmware memory location.  A board can also optionally define
# TEXT1_ADDR to split the firmware into two sections; see below for details.
TEXT0_ADDR ?= 0x00000000

# No TEXT1_ADDR given so put all firmware at TEXT0_ADDR location

TEXT0_SECTIONS ?= .isr_vector .text .data

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(call GENERATE_BIN,$@,$^,$(TEXT0_SECTIONS))

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
GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
GEN_PINS_AD_CONST = $(HEADER_BUILD)/pins_ad_const.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
#GEN_PINS_AF_DEFS = $(HEADER_BUILD)/pins_af_defs.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py

FILE2H = $(TOP)/tools/file2h.py

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_CXX) $(SHARED_SRC_C)

# Making OBJ use an order-only depenedency on the generated pins.h file
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
$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h $(HEADER_BUILD)/%_af_const.h $(HEADER_BUILD)/%_af_defs.h $(BUILD)/%_qstr.h: $(BOARD_DIR)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) --ad-const $(GEN_PINS_AD_CONST) --af-const $(GEN_PINS_AF_CONST) --af-py $(GEN_PINS_AF_PY) > $(GEN_PINS_SRC)

#$(BUILD)/pins_$(BOARD).o: $(BUILD)/pins_$(BOARD).c
#	$(call compile_c)

CMSIS_MCU_HDR = $(CMSIS_DIR)/$(CMSIS_MCU_LOWER).h

include $(TOP)/py/mkrules.mk
