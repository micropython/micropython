# Select the board to build for.
ifeq ($(BOARD),)
  $(info You must provide a BOARD parameter with 'BOARD=')
  $(info Possible values are:)
  $(info $(sort $(subst /.,,$(subst boards/,,$(wildcard boards/*/.)))))
  $(error BOARD not defined)
else
  ifeq ($(wildcard boards/$(BOARD)/.),)
    $(error Invalid BOARD specified)
  endif
endif

include boards/$(BOARD)/mpconfigboard.mk

SD_LOWER = $(shell echo $(SD) | tr '[:upper:]' '[:lower:]')

# Build directory with SD
BUILD ?= $(if $(SD),build-$(BOARD)-$(SD_LOWER),build-$(BOARD))

include ../../py/mkenv.mk
-include mpconfigport.mk

ifneq ($(SD), )
	include drivers/bluetooth/bluetooth_common.mk
endif

FROZEN_MPY_DIR = freeze

# include py core make definitions
include ../../py/py.mk

include $(TOP)/supervisor/supervisor.mk

FATFS_DIR = lib/oofatfs

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I$(BUILD)/genhdr
INC += -I./../../lib/cmsis/inc
INC += -I./boards/$(BOARD)
INC += -I./modules/ubluepy
INC += -I./modules/ble
INC += -I./nrfx
INC += -I./nrfx/hal
INC += -I./nrfx/mdk
INC += -I./nrfx/drivers/include
INC += -I../../lib/mp-readline
INC += -I./drivers/bluetooth
INC += -I./drivers
INC += -I./peripherals
INC += -I../../lib/tinyusb/src
INC += -I./usb

NRF_DEFINES += -DCONFIG_GPIO_AS_PINRESET

CFLAGS += -mthumb -mabi=aapcs -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS += $(INC) -Wall -Werror -ansi -std=gnu99 -nostdlib $(COPT) $(NRF_DEFINES) $(CFLAGS_MOD)
CFLAGS += -fno-strict-aliasing
CFLAGS += -fstack-usage -fno-builtin -fshort-enums
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -D__START=main

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

LIBM_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libm.a)
LIBC_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libc.a)
LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

LIBS := -L $(dir $(LIBM_FILE_NAME)) -lm
LIBS += -L $(dir $(LIBC_FILE_NAME)) -lc
LIBS += -L $(dir $(LIBGCC_FILE_NAME)) -lgcc

SRC_NRFX = $(addprefix nrfx/,\
	drivers/src/nrfx_power.c \
	drivers/src/nrfx_spim.c \
	drivers/src/nrfx_twim.c \
	drivers/src/nrfx_uart.c \
	)

SRC_C += \
	background.c \
	fatfs_port.c \
	internal_flash.c \
	mphalport.c \
	tick.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	device/$(MCU_VARIANT)/startup_$(MCU_SUB_VARIANT).c \
	drivers/bluetooth/ble_drv.c \
	drivers/bluetooth/ble_uart.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/option/ccsbcs.c \
	lib/timeutils/timeutils.c \
	lib/utils/buffer_helper.c \
	lib/utils/context_manager_helpers.c \
	lib/utils/interrupt_char.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c \
	lib/utils/sys_stdio_mphal.c \
	nrfx/hal/nrf_nvmc.c \
	nrfx/mdk/system_$(MCU_SUB_VARIANT).c \
	peripherals/nrf/cache.c \
	peripherals/nrf/clocks.c \
	peripherals/nrf/$(MCU_CHIP)/pins.c \
	peripherals/nrf/$(MCU_CHIP)/power.c \
	supervisor/shared/memory.c

DRIVERS_SRC_C += $(addprefix modules/,\
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
	)

SRC_COMMON_HAL += \
	analogio/AnalogIn.c \
	analogio/AnalogOut.c \
	analogio/__init__.c \
	board/__init__.c \
	busio/I2C.c \
	busio/SPI.c \
	busio/UART.c \
	busio/__init__.c\
	digitalio/DigitalInOut.c \
	digitalio/__init__.c \
	microcontroller/Pin.c \
	microcontroller/Processor.c \
	microcontroller/__init__.c \
	neopixel_write/__init__.c \
	os/__init__.c \
	pulseio/PWMOut.c \
	pulseio/PulseIn.c \
	pulseio/PulseOut.c \
	pulseio/__init__.c \
	storage/__init__.c \
	supervisor/Runtime.c \
	supervisor/__init__.c \
	time/__init__.c \

ifneq ($(SD), )
SRC_COMMON_HAL += \
	bleio/__init__.c \
	bleio/Adapter.c
endif

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
	bitbangio/OneWire.c \
	random/__init__.c \

# USB source files for nrf52840
ifeq ($(MCU_SUB_VARIANT),nrf52840)

SRC_C += \
	usb/usb.c \
	usb/usb_msc_flash.c \
	usb/usb_desc.c \
	lib/tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.c \
	lib/tinyusb/src/portable/nordic/nrf5x/hal_nrf5x.c \
	lib/tinyusb/src/common/tusb_fifo.c \
	lib/tinyusb/src/device/usbd.c \
	lib/tinyusb/src/class/msc/msc_device.c \
	lib/tinyusb/src/class/cdc/cdc_device.c \
	lib/tinyusb/src/class/hid/hid_device.c \
	lib/tinyusb/src/tusb.c \

SRC_COMMON_HAL += \
	usb_hid/__init__.c \
	usb_hid/Device.c \

endif


SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_BINDINGS)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE))

SRC_S = supervisor/cpu.s

FROZEN_MPY_PY_FILES := $(shell find -L $(FROZEN_MPY_DIR) -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))

OBJ += $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_NRFX:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

.phony: all flash sd binary hex

all: binary hex uf2

OUTPUT_FILENAME = firmware

## Create binary .bin file from the .out file
binary: $(BUILD)/$(OUTPUT_FILENAME).bin

$(BUILD)/$(OUTPUT_FILENAME).bin: $(BUILD)/$(OUTPUT_FILENAME).elf
	$(OBJCOPY) -O binary $< $@

## Create binary .hex file from the .out file
hex: $(BUILD)/$(OUTPUT_FILENAME).hex

$(BUILD)/$(OUTPUT_FILENAME).hex: $(BUILD)/$(OUTPUT_FILENAME).elf
	$(OBJCOPY) -O ihex $< $@

## Create uf2 file
uf2: $(BUILD)/$(OUTPUT_FILENAME).hex
	$(ECHO) "Create $(OUTPUT_FILENAME).uf2"
	$(PYTHON2) $(TOP)/tools/uf2/utils/uf2conv.py -f 0xADA52840 -c -o "$(BUILD)/$(OUTPUT_FILENAME).uf2" $^

#####################
# Flash with debugger
#####################
FLASHER ?=

ifeq ($(FLASHER),)

# Also update to bootloader settting to validate application and skip checksum ( app valid = 0x0001, crc = 0x0000 )
flash: $(BUILD)/$(OUTPUT_FILENAME).hex
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --erasepage $(BOOT_SETTING_ADDR) -f $(MCU_VARIANT)
	nrfjprog --memwr $(BOOT_SETTING_ADDR) --val 0x00000001 -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	nrfjprog --eraseall -f $(MCU_VARIANT)
	nrfjprog --program $(SOFTDEV_HEX) -f $(MCU_VARIANT)
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

else ifeq ($(FLASHER), pyocd)

flash: $(BUILD)/$(OUTPUT_FILENAME).hex
	pyocd-flashtool -t $(MCU_SUB_VARIANT) $< --sector_erase
	pyocd-tool -t $(MCU_SUB_VARIANT) erase $(BOOT_SETTING_ADDR)
	pyocd-tool -t $(MCU_SUB_VARIANT) write32 $(BOOT_SETTING_ADDR) 0x00000001
	pyocd-tool -t $(MCU_SUB_VARIANT) reset

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	pyocd-flashtool -t $(MCU_SUB_VARIANT) --chip_erase
	pyocd-flashtool -t $(MCU_SUB_VARIANT) $(SOFTDEV_HEX)
	pyocd-flashtool -t $(MCU_SUB_VARIANT) $< --sector_erase
	pyocd-tool -t $(MCU_SUB_VARIANT) reset $(BOOT_SETTING_ADDR)

endif

#####################
# Flash with DFU
#####################
.phony: dfu-gen dfu-flash

NRFUTIL = adafruit-nrfutil

ifeq ($(MCU_SUB_VARIANT),nrf52840)
	DFU_TOUCH = --touch 1200
else
	DFU_TOUCH =
endif

check_defined = \
    $(strip $(foreach 1,$1, \
    $(call __check_defined,$1,$(strip $(value 2)))))
__check_defined = \
    $(if $(value $1),, \
    $(error Undefined make flag: $1$(if $2, ($2))))

## Flash with DFU serial
dfu-flash: $(BUILD)/dfu-package.zip
	@:$(call check_defined, SERIAL, example: SERIAL=/dev/ttyUSB0)
	$(NRFUTIL) --verbose dfu serial --package $^ -p $(SERIAL) -b 115200 --singlebank $(DFU_TOUCH)

## Create DFU package file
dfu-gen: $(BUILD)/dfu-package.zip

$(BUILD)/dfu-package.zip: $(BUILD)/$(OUTPUT_FILENAME).hex
	$(NRFUTIL) dfu genpkg --sd-req 0xFFFE --dev-type 0x0052 --application $^ $(BUILD)/dfu-package.zip

$(BUILD)/$(OUTPUT_FILENAME).elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_MOD) $(DRIVERS_SRC_C) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)

# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

AF_FILE = $(MCU_VARIANT)_af.csv
PREFIX_FILE = boards/$(MCU_VARIANT)_prefix.c

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
