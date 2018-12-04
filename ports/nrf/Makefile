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

CLI_SD := $(SD)

include boards/$(BOARD)/mpconfigboard.mk

SD_LOWER = $(shell echo $(SD) | tr '[:upper:]' '[:lower:]')

# Build directory with SD if it's different from the default.
BUILD ?= $(if $(CLI_SD),build-$(BOARD)-$(SD_LOWER),build-$(BOARD))

include ../../py/mkenv.mk
-include mpconfigport.mk

ifneq ($(SD), )
	include bluetooth/bluetooth_common.mk
endif

FROZEN_MPY_DIR = freeze

# include py core make definitions
include ../../py/py.mk

ifneq ($(MCU_SUB_VARIANT),nrf52840)
USB = FALSE
endif

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
INC += -I./bluetooth
INC += -I./peripherals
INC += -I../../lib/mp-readline
INC += -I../../lib/tinyusb/src
INC += -I../../supervisor/shared/usb

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

# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_NRF5X -DCFG_TUD_CDC_RX_BUFSIZE=1024 -DCFG_TUD_CDC_TX_BUFSIZE=1024 -DCFG_TUD_MSC_BUFSIZE=4096

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
	drivers/src/nrfx_timer.c \
	drivers/src/nrfx_twim.c \
	drivers/src/nrfx_uarte.c \
	)

ifdef EXTERNAL_FLASH_DEVICES
	ifeq ($(QSPI_FLASH_FILESYSTEM),1)
		SRC_NRFX += nrfx/drivers/src/nrfx_qspi.c
	endif
endif


SRC_C += \
	background.c \
	fatfs_port.c \
	mphalport.c \
	tick.c \
	board_busses.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	device/$(MCU_VARIANT)/startup_$(MCU_SUB_VARIANT).c \
	bluetooth/ble_drv.c \
	bluetooth/ble_uart.c \
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
	peripherals/nrf/timers.c \
	supervisor/shared/memory.c


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
	supervisor/Runtime.c \
	supervisor/__init__.c \
	time/__init__.c \
	touchio/__init__.c \
	touchio/TouchIn.c \

ifneq ($(SD), )
SRC_COMMON_HAL += \
	bleio/__init__.c \
	bleio/Adapter.c \
	bleio/Characteristic.c \
	bleio/Descriptor.c \
	bleio/Device.c \
	bleio/Scanner.c \
	bleio/Service.c \
	bleio/UUID.c
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

ifneq ($(SD), )
SRC_BINDINGS_ENUMS += \
	bleio/Address.c \
	bleio/AddressType.c \
	bleio/AdvertisementData.c \
	bleio/ScanEntry.c \
	bleio/UUIDType.c
endif

SRC_SHARED_MODULE = \
	os/__init__.c \
	random/__init__.c \
	struct/__init__.c \
	gamepad/__init__.c \
	gamepad/GamePad.c \
	bitbangio/__init__.c \
	bitbangio/I2C.c \
	bitbangio/OneWire.c \
	bitbangio/SPI.c \
	busio/OneWire.c \
	storage/__init__.c

#	uheap/__init__.c \
	ustack/__init__.c

# USB source files for nrf52840
ifeq ($(MCU_SUB_VARIANT),nrf52840)

SRC_C += \
	lib/tinyusb/src/portable/nordic/nrf5x/dcd_nrf5x.c \
	lib/tinyusb/src/portable/nordic/nrf5x/hal_nrf5x.c

SRC_SHARED_MODULE += \
	usb_hid/__init__.c \
	usb_hid/Device.c \

endif

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
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
	pyocd-flashtool -t $(MCU_VARIANT) $< --sector_erase
	#pyocd-tool -t $(MCU_VARIANT) erase $(BOOT_SETTING_ADDR)
	pyocd-tool -t $(MCU_VARIANT) write32 $(BOOT_SETTING_ADDR) 0x00000001
	pyocd-tool -t $(MCU_VARIANT) reset

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	pyocd-flashtool -t $(MCU_VARIANT) --chip_erase
	pyocd-flashtool -t $(MCU_VARIANT) $(SOFTDEV_HEX)
	pyocd-flashtool -t $(MCU_VARIANT) $< --sector_erase
	pyocd-tool -t $(MCU_VARIANT) reset $(BOOT_SETTING_ADDR)

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

# You must have $^ here because it deduplicates entries in $(OBJ)
$(BUILD)/$(OUTPUT_FILENAME).elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ -Wl,--start-group $(LIBS) -Wl,--end-group
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
