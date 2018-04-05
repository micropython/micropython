# Select the board to build for: if not given on the command line,
# then default to pca10040.
BOARD ?= pca10040
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# If SoftDevice is selected, try to use that one.
SD ?=
SD_LOWER = $(shell echo $(SD) | tr '[:upper:]' '[:lower:]')

# TODO: Verify that it is a valid target.

include boards/$(BOARD)/mpconfigboard.mk

ifeq ($(SD), )
	# If the build directory is not given, make it reflect the board name.
	BUILD ?= build-$(BOARD)
	include ../../py/mkenv.mk
else
	# If the build directory is not given, make it reflect the board name.
	BUILD ?= build-$(BOARD)-$(SD_LOWER)
	include ../../py/mkenv.mk

	LD_FILES += boards/$(SD_LOWER)_$(SOFTDEV_VERSION).ld
	include drivers/bluetooth/bluetooth_common.mk
endif

LD_FILES += boards/memory.ld boards/common.ld

ifneq ($(LD_FILE),)
	# Use custom LD file
	LD_FILES = $(LD_FILE)
endif

-include boards/$(BOARD)/modules/boardmodules.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(BUILD)/pins_qstr.h

# include py core make definitions
include ../../py/py.mk


FATFS_DIR = lib/oofatfs
MPY_CROSS = ../../mpy-cross/mpy-cross
MPY_TOOL = ../../tools/mpy-tool.py

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I./../../lib/cmsis/inc
INC += -I./modules/machine
INC += -I./modules/ubluepy
INC += -I./modules/music
INC += -I./modules/random
INC += -I./modules/ble
INC += -I../../lib/mp-readline
INC += -I./drivers/bluetooth
INC += -I./drivers
INC += -I../../lib/nrfx/
INC += -I../../lib/nrfx/drivers
INC += -I../../lib/nrfx/drivers/include
INC += -I../../lib/nrfx/mdk
INC += -I../../lib/nrfx/hal

MCU_VARIANT_UPPER = $(shell echo $(MCU_VARIANT) | tr '[:lower:]' '[:upper:]')
MCU_SUB_VARIANT_UPPER = $(shell echo $(MCU_SUB_VARIANT) | tr '[:lower:]' '[:upper:]')

# Figure out correct system file to use base on chip sub-variant name.
SYSTEM_C_SRC :=
ifeq ($(MCU_SUB_VARIANT),nrf51822)
	SYSTEM_C_SRC += $(addprefix lib/nrfx/mdk/, system_nrf51.c)
        NRF_DEFINES += -D$(MCU_VARIANT_UPPER)
else ifeq ($(MCU_SUB_VARIANT),nrf52832)
	SYSTEM_C_SRC += $(addprefix lib/nrfx/mdk/, system_nrf52.c)
        NRF_DEFINES += -D$(MCU_VARIANT_UPPER)
else ifeq ($(MCU_SUB_VARIANT),nrf52840)
	SYSTEM_C_SRC += $(addprefix lib/nrfx/mdk/, system_nrf52840.c)
	# Do not pass MCU_VARIANT_UPPER flag, as NRF52 defines NRF52832 only.
endif

NRF_DEFINES += -D$(MCU_SUB_VARIANT_UPPER)
NRF_DEFINES += -DCONFIG_GPIO_AS_PINRESET

CFLAGS_CORTEX_M = -mthumb -mabi=aapcs -fsingle-precision-constant -Wdouble-promotion

CFLAGS_MCU_m4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS_MCU_m0 = $(CFLAGS_CORTEX_M) --short-enums -mtune=cortex-m0 -mcpu=cortex-m0 -mfloat-abi=soft -fno-builtin

LTO ?= 1
ifeq ($(LTO),1)
CFLAGS_LTO += -flto
else
CFLAGS_LTO += -Wl,--gc-sections -ffunction-sections -fdata-sections
endif


CFLAGS += $(CFLAGS_MCU_$(MCU_SERIES)) 
CFLAGS += $(INC) -Wall -Werror -g -ansi -std=gnu99 -nostdlib $(COPT) $(NRF_DEFINES) $(CFLAGS_MOD)
CFLAGS += -fno-strict-aliasing
CFLAGS += -fstack-usage
CFLAGS += -Iboards/$(BOARD)
CFLAGS += -DNRF5_HAL_H='<$(MCU_VARIANT)_hal.h>'
CFLAGS += $(CFLAGS_LTO)

LDFLAGS = $(CFLAGS)
LDFLAGS += -Xlinker -Map=$(@:.elf=.map)
LDFLAGS += -mthumb -mabi=aapcs $(addprefix -T,$(LD_FILES)) -L boards/

#Debugging/Optimization
ifeq ($(DEBUG), 1)
#ASMFLAGS += -g -gtabs+
CFLAGS += -O0 -ggdb
LDFLAGS += -O0
else
CFLAGS += -Os -DNDEBUG
LDFLAGS += -Os
endif

LIBS = \

ifeq ($(MCU_VARIANT), nrf52)
LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

LIBS += -L $(dir $(LIBGCC_FILE_NAME)) -lgcc


SRC_LIB += $(addprefix lib/,\
        libm/math.c \
        libm/fmodf.c \
        libm/nearbyintf.c \
        libm/ef_sqrt.c \
        libm/kf_rem_pio2.c \
        libm/kf_sin.c \
        libm/kf_cos.c \
        libm/kf_tan.c \
        libm/ef_rem_pio2.c \
        libm/sf_sin.c \
        libm/sf_cos.c \
        libm/sf_tan.c \
        libm/sf_frexp.c \
        libm/sf_modf.c \
        libm/sf_ldexp.c \
        libm/asinfacosf.c \
        libm/atanf.c \
        libm/atan2f.c \
	)

endif

SRC_LIB += $(addprefix lib/,\
	libc/string0.c \
	mp-readline/readline.c \
	utils/pyexec.c \
	utils/interrupt_char.c \
	timeutils/timeutils.c \
	oofatfs/ff.c \
	oofatfs/option/unicode.c \
	)

SRC_NRFX += $(addprefix lib/nrfx/drivers/src/,\
	prs/nrfx_prs.c \
	nrfx_uart.c \
	nrfx_adc.c \
	nrfx_saadc.c \
	nrfx_rng.c \
	nrfx_twi.c \
	nrfx_spi.c \
	nrfx_spim.c \
	nrfx_rtc.c \
	nrfx_timer.c \
	nrfx_pwm.c \
	nrfx_gpiote.c \
	)

SRC_NRFX_HAL += $(addprefix lib/nrfx/hal/,\
	nrf_nvmc.c \
	)

SRC_C += \
	main.c \
	mphalport.c \
	help.c \
	gccollect.c \
	pin_named_pins.c \
	fatfs_port.c \
	drivers/flash.c \
	drivers/softpwm.c \
	drivers/ticker.c \
	drivers/bluetooth/ble_drv.c \
	drivers/bluetooth/ble_uart.c \

DRIVERS_SRC_C += $(addprefix modules/,\
	machine/modmachine.c \
	machine/uart.c \
	machine/spi.c \
	machine/i2c.c \
	machine/adc.c \
	machine/pin.c \
	machine/timer.c \
	machine/rtcounter.c \
	machine/pwm.c \
	machine/led.c \
	machine/temp.c \
	uos/moduos.c \
	uos/microbitfs.c \
	utime/modutime.c \
	pyb/modpyb.c \
	ubluepy/modubluepy.c \
	ubluepy/ubluepy_peripheral.c \
	ubluepy/ubluepy_service.c \
	ubluepy/ubluepy_characteristic.c \
	ubluepy/ubluepy_uuid.c \
	ubluepy/ubluepy_delegate.c \
	ubluepy/ubluepy_constants.c \
	ubluepy/ubluepy_descriptor.c \
	ubluepy/ubluepy_scanner.c \
	ubluepy/ubluepy_scan_entry.c \
	music/modmusic.c \
	music/musictunes.c \
	ble/modble.c \
	random/modrandom.c \
	)

# Custom micropython startup file with smaller interrupt vector table
# than the file provided in nrfx.
SRC_C += \
	device/startup_$(MCU_SUB_VARIANT).c \

ifneq ($(FROZEN_MPY_DIR),)
FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))
endif

OBJ += $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_NRFX:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_NRFX_HAL:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SYSTEM_C_SRC:.c=.o))
OBJ += $(BUILD)/pins_gen.o

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

.PHONY: all flash sd binary hex

all: binary hex

OUTPUT_FILENAME = firmware

## Create binary .bin file from the .out file
binary: $(BUILD)/$(OUTPUT_FILENAME).bin

$(BUILD)/$(OUTPUT_FILENAME).bin: $(BUILD)/$(OUTPUT_FILENAME).elf
	$(OBJCOPY) -O binary $< $@

## Create binary .hex file from the .out file
hex: $(BUILD)/$(OUTPUT_FILENAME).hex

$(BUILD)/$(OUTPUT_FILENAME).hex: $(BUILD)/$(OUTPUT_FILENAME).elf
	$(OBJCOPY) -O ihex $< $@

FLASHER ?=

ifeq ($(FLASHER),)

flash: $(BUILD)/$(OUTPUT_FILENAME).hex
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	nrfjprog --eraseall -f $(MCU_VARIANT)
	nrfjprog --program $(SOFTDEV_HEX) -f $(MCU_VARIANT)
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

else ifeq ($(FLASHER), pyocd)

flash: $(BUILD)/$(OUTPUT_FILENAME).hex
	pyocd-flashtool -t $(MCU_VARIANT) $<

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	pyocd-flashtool -t $(MCU_VARIANT) --chip_erase
	pyocd-flashtool -t $(MCU_VARIANT) $(SOFTDEV_HEX)
	pyocd-flashtool -t $(MCU_VARIANT) $<

endif

$(BUILD)/$(OUTPUT_FILENAME).elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_MOD) $(SRC_LIB) $(DRIVERS_SRC_C) $(SRC_BOARD_MODULES)

# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

# Making OBJ use an order-only depenedency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(HEADER_BUILD)/pins.h

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h $(HEADER_BUILD)/%_af_const.h $(BUILD)/%_qstr.h: boards/$(BOARD)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) --af-const $(GEN_PINS_AF_CONST) --af-py $(GEN_PINS_AF_PY) > $(GEN_PINS_SRC)

$(BUILD)/pins_gen.o: $(BUILD)/pins_gen.c
	$(call compile_c)

MAKE_PINS = boards/make-pins.py
BOARD_PINS = boards/$(BOARD)/pins.csv
AF_FILE = $(MCU_VARIANT)_af.csv
PREFIX_FILE = boards/$(MCU_VARIANT)_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_gen.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py

ifneq ($(FROZEN_DIR),)
# To use frozen source modules, put your .py files in a subdirectory (eg scripts/)
# and then invoke make with FROZEN_DIR=scripts (be sure to build from scratch).
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
endif

ifneq ($(FROZEN_MPY_DIR),)
# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen (be sure to build from scratch).
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
endif

include ../../py/mkrules.mk

