# Select the board to build for: if not given on the command line,
# then default to feather52.
BOARD ?= feather52
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif

# If SoftDevice is selected, try to use that one.
# Default to SD132 (exact version can be set with SOFTDEV_VERSION)
SD ?= s132
SD_LOWER = $(shell echo $(SD) | tr '[:upper:]' '[:lower:]')

# TODO: Verify that it is a valid target.

ifeq ($(SD), )
	# If the build directory is not given, make it reflect the board name.
	BUILD ?= build-$(BOARD)
	include ../../py/mkenv.mk
	include boards/$(BOARD)/mpconfigboard.mk
	-include mpconfigport.mk

else
	# If the build directory is not given, make it reflect the board name.
	BUILD ?= build-$(BOARD)-$(SD_LOWER)
	include ../../py/mkenv.mk
	include boards/$(BOARD)/mpconfigboard_$(SD_LOWER).mk
	-include mpconfigport.mk

	include drivers/bluetooth/bluetooth_common.mk
endif


# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(BUILD)/pins_qstr.h

FROZEN_MPY_DIR = freeze

# include py core make definitions
include ../../py/py.mk

include $(TOP)/supervisor/supervisor.mk

FATFS_DIR = lib/oofatfs

CROSS_COMPILE = arm-none-eabi-

MCU_VARIANT_UPPER = $(shell echo $(MCU_VARIANT) | tr '[:lower:]' '[:upper:]')

INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I./../../lib/cmsis/inc
INC += -I./device
INC += -I./device/$(MCU_VARIANT)
INC += -I./hal
INC += -I./hal/$(MCU_VARIANT)
INC += -I./modules/machine
INC += -I./modules/ubluepy
INC += -I./modules/music
INC += -I./modules/random
INC += -I./modules/ble
INC += -I../../lib/mp-readline
INC += -I./drivers/bluetooth
INC += -I./drivers

NRF_DEFINES += -D$(MCU_VARIANT_UPPER)
NRF_DEFINES += -DCONFIG_GPIO_AS_PINRESET

CFLAGS_CORTEX_M = -mthumb -mabi=aapcs -fsingle-precision-constant -Wdouble-promotion

CFLAGS_MCU_m4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS_MCU_m0 = $(CFLAGS_CORTEX_M) --short-enums -mtune=cortex-m0 -mcpu=cortex-m0 -mfloat-abi=soft -fno-builtin


CFLAGS += $(CFLAGS_MCU_$(MCU_SERIES))
CFLAGS += $(INC) -Wall -Werror -ansi -std=gnu99 -nostdlib $(COPT) $(NRF_DEFINES) $(CFLAGS_MOD)
CFLAGS += -fno-strict-aliasing
CFLAGS += -fstack-usage
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -Iboards/$(BOARD)
CFLAGS += -DNRF5_HAL_H='<$(MCU_VARIANT)_hal.h>'

LDFLAGS = $(CFLAGS)
LDFLAGS += -Xlinker -Map=$(@:.elf=.map)
LDFLAGS += -mthumb -mabi=aapcs -T $(LD_FILE) -L boards/
LDFLAGS += -Wl,--gc-sections

#Debugging/Optimization
ifeq ($(DEBUG), 1)
#ASMFLAGS += -g -gtabs+
CFLAGS += -O1 -ggdb
LDFLAGS += -O1
else
CFLAGS += -Os -DNDEBUG
LDFLAGS += -Os
endif

LIBS = \

ifeq ($(MCU_VARIANT), nrf52)
LIBM_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libm.a)
LIBC_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libc.a)
LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

LIBS += -L $(dir $(LIBM_FILE_NAME)) -lm
LIBS += -L $(dir $(LIBC_FILE_NAME)) -lc
LIBS += -L $(dir $(LIBGCC_FILE_NAME)) -lgcc
endif

SRC_HAL = $(addprefix hal/,\
	hal_uart.c \
	hal_uarte.c \
	hal_spi.c \
	hal_spie.c \
	hal_time.c \
	hal_timer.c \
	hal_twi.c \
	hal_adc.c \
	hal_adce.c \
	hal_temp.c \
	hal_gpio.c \
	hal_rng.c \
	)

ifeq ($(MCU_VARIANT), nrf52)
SRC_HAL += $(addprefix hal/,\
	hal_pwm.c \
	)
endif


SRC_C += \
	mphalport.c \
	help.c \
	pin_named_pins.c \
	fatfs_port.c \
	fifo.c \
	tick.c \
	drivers/softpwm.c \
	drivers/ticker.c \
	drivers/bluetooth/ble_drv.c \
	drivers/bluetooth/ble_uart.c \
	boards/$(BOARD)/board.c \
	device/$(MCU_VARIANT)/system_$(MCU_SUB_VARIANT).c \
	device/$(MCU_VARIANT)/startup_$(MCU_SUB_VARIANT).c \
	lib/oofatfs/ff.c \
	lib/oofatfs/option/ccsbcs.c \
	lib/timeutils/timeutils.c \
	lib/utils/buffer_helper.c \
	lib/utils/context_manager_helpers.c \
	lib/utils/interrupt_char.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	internal_flash.c \


DRIVERS_SRC_C += $(addprefix modules/,\
	machine/modmachine.c \
	machine/uart.c \
	machine/spi.c \
	machine/i2c.c \
	machine/adc.c \
	machine/pin.c \
	machine/timer.c \
	machine/pwm.c \
	machine/led.c \
	machine/temp.c \
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


SRC_COMMON_HAL += \
	board/__init__.c \
	digitalio/__init__.c \
	digitalio/DigitalInOut.c \
	microcontroller/__init__.c \
	microcontroller/Pin.c \
	microcontroller/Processor.c \
	os/__init__.c \
	time/__init__.c \
	analogio/__init__.c \
	analogio/AnalogIn.c \
	analogio/AnalogOut.c \
	busio/__init__.c\
	busio/I2C.c \
	busio/SPI.c \
	busio/UART.c \
	pulseio/__init__.c \
	pulseio/PulseIn.c \
	pulseio/PulseOut.c \
	pulseio/PWMOut.c \
	storage/__init__.c \
        supervisor/__init__.c \
        supervisor/Runtime.c \

# These don't have corresponding files in each port but are still located in
# shared-bindings to make it clear what the contents of the modules are.
SRC_BINDINGS_ENUMS = \
	busio/OneWire.c \
	digitalio/Direction.c \
	digitalio/DriveMode.c \
	digitalio/Pull.c \
	microcontroller/RunMode.c \
	help.c \
	math/__init__.c \
	util.c

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE = \
	os/__init__.c \
	random/__init__.c \
	storage/__init__.c \
	struct/__init__.c \
	gamepad/__init__.c \
	gamepad/GamePad.c \
	bitbangio/__init__.c \
	bitbangio/I2C.c \
	bitbangio/OneWire.c \
	bitbangio/SPI.c \
	busio/OneWire.c \

#	uheap/__init__.c \
	ustack/__init__.c

SRC_SHARED_BINDINGS = \
	struct/__init__.c \
	gamepad/__init__.c \
	gamepad/GamePad.c \
	bitbangio/__init__.c \
	bitbangio/I2C.c \
	bitbangio/SPI.c \
	bitbangio/OneWire.c


SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_BINDINGS)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE))

FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))

OBJ += $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(BUILD)/pins_gen.o
OBJ += $(addprefix $(BUILD)/, $(SRC_HAL:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

.phony: all flash sd binary hex

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
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(DRIVERS_SRC_C) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)

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
$(BUILD)/%_gen.c $(HEADER_BUILD)/%.h $(HEADER_BUILD)/%_af_const.h $(BUILD)/%_qstr.h: boards/$(BOARD)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
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

include $(TOP)/py/mkrules.mk
