# Select the board to build for:
ifdef BOARD_DIR
# Custom board path - remove trailing slash and get the final component of
# the path as the board name.
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
# If not given on the command line, then default to PCA10040.
BOARD ?= PCA10040
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

# If SoftDevice is selected, try to use that one.
SD ?=
SD_LOWER = $(shell echo $(SD) | tr '[:upper:]' '[:lower:]')

# TODO: Verify that it is a valid target.

include $(BOARD_DIR)/mpconfigboard.mk

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

ifneq ($(BOOTLOADER),)
	BOOTLOADER_UPPER = $(shell echo $(BOOTLOADER) | tr '[:lower:]' '[:upper:]')
	# Use additional bootloader linker script
	LD_FILES += boards/$(MCU_SUB_VARIANT)_$(BOOTLOADER)_$(BOOTLOADER_VERSION_MAJOR).$(BOOTLOADER_VERSION_MINOR).x.ld
	CFLAGS += -DBOOTLOADER_$(BOOTLOADER_UPPER)_VERSION=$(BOOTLOADER_MAJOR)$(BOOTLOADER_MINOR)
endif

LD_FILES += boards/memory.ld boards/common.ld

ifneq ($(LD_FILE),)
	# Use custom LD file
	LD_FILES = $(LD_FILE)
endif

-include $(BOARD_DIR)/modules/boardmodules.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
ifeq ($(DEBUG), 0)
MICROPY_ROM_TEXT_COMPRESSION ?= 1
endif

FROZEN_MANIFEST ?= modules/manifest.py

# include py core make definitions
include ../../py/py.mk
include ../../extmod/extmod.mk

GIT_SUBMODULES += lib/nrfx lib/tinyusb

MICROPY_VFS_FAT ?= 0

CROSS_COMPILE ?= arm-none-eabi-

INC += -I.
INC += -I../..
INC += -I$(BUILD)
INC += -I./../../lib/cmsis/inc
INC += -I./modules/machine
INC += -I./modules/ubluepy
INC += -I./modules/music
INC += -I./modules/ble
INC += -I./modules/board
INC += -I./modules/nrf
INC += -I../../shared/readline
INC += -I./drivers/bluetooth
INC += -I./drivers
INC += -I../../lib/nrfx/
INC += -I../../lib/nrfx/drivers
INC += -I../../lib/nrfx/drivers/include
INC += -I../../lib/nrfx/mdk
INC += -I../../lib/nrfx/hal
INC += -I../../lib/nrfx/drivers/src/

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
else ifeq ($(MCU_SUB_VARIANT),nrf9160)
        SYSTEM_C_SRC += $(addprefix lib/nrfx/mdk/, system_nrf9160.c)
        NRF_DEFINES += -D$(MCU_VARIANT_UPPER)
endif

NRF_DEFINES += -D$(MCU_SUB_VARIANT_UPPER)
NRF_DEFINES += -DCONFIG_GPIO_AS_PINRESET

MAKE_PINS = boards/make-pins.py
BOARD_PINS = $(BOARD_DIR)/pins.csv
AF_FILE = $(MCU_VARIANT)_af.csv
PREFIX_FILE = boards/$(MCU_VARIANT)_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_gen.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h

CFLAGS_CORTEX_M = -mthumb -mabi=aapcs -fsingle-precision-constant -Wdouble-promotion

CFLAGS_MCU_m33 = $(CFLAGS_CORTEX_M) -mcpu=cortex-m33 -march=armv8-m.main+dsp -mcmse -mfpu=fpv5-sp-d16 -mfloat-abi=hard

CFLAGS_MCU_m4 = $(CFLAGS_CORTEX_M) -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CFLAGS_MCU_m0 = $(CFLAGS_CORTEX_M) -fshort-enums -mtune=cortex-m0 -mcpu=cortex-m0 -mfloat-abi=soft

LTO ?= 1
ifeq ($(LTO),1)
CFLAGS += -flto
else
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections
endif

CFLAGS += $(CFLAGS_MCU_$(MCU_SERIES))
CFLAGS += $(INC) -Wall -Werror -ansi -std=c11 -nostdlib $(COPT) $(NRF_DEFINES) $(CFLAGS_EXTRA)
CFLAGS += -fno-strict-aliasing
CFLAGS += -I$(BOARD_DIR)
CFLAGS += -DNRF5_HAL_H='<$(MCU_VARIANT)_hal.h>'

LDFLAGS += $(CFLAGS)
LDFLAGS += -Xlinker -Map=$(@:.elf=.map)
LDFLAGS += -mthumb -mabi=aapcs $(addprefix -T,$(LD_FILES)) -L boards/

ifneq ($(FS_SIZE),)
LDFLAGS += -Wl,'--defsym=_fs_size=$(FS_SIZE)'
endif

#Debugging/Optimization
ifeq ($(DEBUG), 1)
#ASMFLAGS += -g -gtabs+
CFLAGS += -g -O0
LDFLAGS += -O0
else
ifneq ($(LTO), 1)
CFLAGS += -g  # always include debug info in the ELF, unless LTO is on
endif
CFLAGS += -Os -DNDEBUG
LDFLAGS += -Os
endif

ifeq ($(MCU_VARIANT), nrf52)

SRC_LIB_C += $(SRC_LIB_LIBM_C)
SRC_LIB_C += $(SRC_LIB_LIBM_SQRT_SW_C)

endif

ifeq ($(MCU_VARIANT), nrf91)

SRC_LIB_C += $(SRC_LIB_LIBM_C)
SRC_LIB_C += $(SRC_LIB_LIBM_SQRT_SW_C)

include drivers/secureboot/secureboot.mk

endif

SRC_SHARED_C += $(addprefix shared/,\
	libc/string0.c \
	readline/readline.c \
	runtime/pyexec.c \
	runtime/sys_stdio_mphal.c \
	runtime/interrupt_char.c \
	tinyusb/mp_usbd_cdc.c \
	timeutils/timeutils.c \
	)

ifeq ($(MICROPY_FATFS), 1)
SRC_LIB_C += $(addprefix lib/,\
        oofatfs/ff.c \
        oofatfs/ffunicode.c \
        )
endif


SRC_NRFX += $(addprefix lib/nrfx/drivers/src/,\
	prs/nrfx_prs.c \
	nrfx_uart.c \
        nrfx_uarte.c \
	nrfx_adc.c \
	nrfx_saadc.c \
	nrfx_temp.c \
	nrfx_rng.c \
	nrfx_twi.c \
        nrfx_twim.c \
	nrfx_spi.c \
	nrfx_spim.c \
	nrfx_rtc.c \
	nrfx_timer.c \
	nrfx_pwm.c \
	nrfx_gpiote.c \
	nrfx_nvmc.c \
	nrfx_power.c \
	nrfx_clock.c \
	)

SRC_C += \
	main.c \
	mphalport.c \
	help.c \
	gccollect.c \
	pin_named_pins.c \
	fatfs_port.c \
	drivers/flash.c \
	drivers/rng.c \
	drivers/softpwm.c \
	drivers/ticker.c \
	drivers/bluetooth/ble_drv.c \
	drivers/bluetooth/ble_uart.c \
	$(wildcard $(BOARD_DIR)/*.c) \

ifeq ($(MCU_SUB_VARIANT), nrf52840)

INC += -I./drivers/usb
INC += -I../../lib/tinyusb/src


# If SoftDevice is selected.
ifneq ($(SD), )
# For external tinyusb drivers to enable SoftDevice mode.
CFLAGS += -DSOFTDEVICE_PRESENT
endif

SRC_C += $(addprefix drivers/usb/,\
	usb_cdc.c \
	usb_descriptors.c \
	)

SRC_C += $(addprefix lib/tinyusb/src/,\
	common/tusb_fifo.c \
	device/usbd.c \
	device/usbd_control.c \
	class/cdc/cdc_device.c \
	tusb.c \
	portable/nordic/nrf5x/dcd_nrf5x.c \
	)

endif

DRIVERS_SRC_C += $(addprefix modules/,\
	machine/spi.c \
	machine/i2c.c \
	machine/pin.c \
	machine/timer.c \
	machine/rtcounter.c \
	machine/temp.c \
	os/microbitfs.c \
	board/modboard.c \
	board/led.c \
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
	nrf/modnrf.c \
	nrf/flashbdev.c \
	)

# Custom micropython startup file with smaller interrupt vector table
# than the file provided in nrfx.
SRC_C += \
	device/startup_$(MCU_SUB_VARIANT).c \

LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
LIBS += -L $(dir $(LIBGCC_FILE_NAME)) -lgcc

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_NRFX:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_NRFX_HAL:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SYSTEM_C_SRC:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_C:.c=.o))
OBJ += $(GEN_PINS_SRC:.c=.o)

$(BUILD)/$(OOFATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

.PHONY: all flash deploy sd binary hex

ifeq ($(MCU_VARIANT), nrf91)
all: binary hex secureboot
else
all: binary hex
endif

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

ifeq ($(MCU_VARIANT), nrf91)

deploy: $(BUILD)/$(OUTPUT_FILENAME).hex $(BUILD)/secureboot.hex
	nrfjprog --program $(BUILD)/secureboot.hex --sectorerase -f $(MCU_VARIANT)
	nrfjprog --program $(BUILD)/$(OUTPUT_FILENAME).hex --sectorerase -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

else

deploy: $(BUILD)/$(OUTPUT_FILENAME).hex
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

endif

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	nrfjprog --eraseall -f $(MCU_VARIANT)
	nrfjprog --program $(SOFTDEV_HEX) -f $(MCU_VARIANT)
	nrfjprog --program $< --sectorerase -f $(MCU_VARIANT)
	nrfjprog --reset -f $(MCU_VARIANT)

else ifeq ($(FLASHER), pyocd)

deploy: $(BUILD)/$(OUTPUT_FILENAME).hex
	pyocd-flashtool -t $(MCU_VARIANT) $<

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	pyocd-flashtool -t $(MCU_VARIANT) --chip_erase
	pyocd-flashtool -t $(MCU_VARIANT) $(SOFTDEV_HEX)
	pyocd-flashtool -t $(MCU_VARIANT) $<

else ifeq ($(FLASHER), idap)

deploy: $(BUILD)/$(OUTPUT_FILENAME).hex
	IDAPnRFPRog $<

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	IDAPnRFPRog $(SOFTDEV_HEX) $<

else ifeq ($(FLASHER), bmp)

BMP_PORT ?= /dev/ttyACM0

deploy: $(BUILD)/$(OUTPUT_FILENAME).elf
	$(Q)$(GDB) -nx --batch \
	    -ex 'target extended-remote $(BMP_PORT)' \
	    -ex 'monitor tpwr enable' \
	    -ex 'monitor swdp_scan' \
	    -ex 'attach 1' \
	    -ex 'set mem inaccessible-by-default off' \
	    -ex 'load' \
	    -ex 'compare-sections' \
	    -ex 'kill' \
	    $<

sd: $(BUILD)/$(OUTPUT_FILENAME).elf
	$(Q)$(GDB) -nx --batch \
	    -ex 'target extended-remote $(BMP_PORT)' \
	    -ex 'monitor tpwr enable' \
	    -ex 'monitor swdp_scan' \
	    -ex 'attach 1' \
	    -ex 'set mem inaccessible-by-default off' \
	    -ex 'monitor erase_mass' \
	    -ex 'load' \
	    -ex 'compare-sections' \
	    -ex 'file $(SOFTDEV_HEX)' \
	    -ex 'load' \
	    -ex 'compare-sections' \
	    -ex 'kill' \
	    $<

else ifeq ($(FLASHER), openocd)

OPENOCD ?= openocd

ifeq ($(MCU_VARIANT), nrf51)
OPENOCD_TARGET ?= target/nrf52.cfg
else ifeq ($(MCU_VARIANT), nrf52)
OPENOCD_TARGET ?= target/nrf52.cfg
else
$(error Unsupported openocd target)
endif

deploy: $(BUILD)/$(OUTPUT_FILENAME).hex
	$(Q)$(OPENOCD) -f interface/cmsis-dap.cfg -f $(OPENOCD_TARGET) -c "init" -c "program $< verify reset" -c "exit"

sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	$(Q)$(OPENOCD) -f interface/cmsis-dap.cfg -f $(OPENOCD_TARGET) -c "init" -c "program $(SOFTDEV_HEX) verify reset" -c "exit"

else ifeq ($(FLASHER), nrfutil)

NRFUTIL_PORT ?= /dev/ttyACM0
NRFUTIL_SD_REQ ?= 0x00

ifeq ($(SD), s140)
NRFUTIL_SD_REQ = 0xB6
endif

.PHONY: nrfutil_dfu_sd nrfutil_dfu_deploy
.NOTPARALLEL: nrfutil_dfu_sd nrfutil_dfu_deploy

# DFU both SD and app in case of target "sd"
sd: nrfutil_dfu_sd nrfutil_dfu_deploy

nrfutil_dfu_sd: $(BUILD)/$(OUTPUT_FILENAME).hex
	$(Q)hexmerge.py -o $(BUILD)/stripped_sd.hex --range=0x1000: $(SOFTDEV_HEX)
	$(Q)nrfutil pkg generate --hw-version 52 --sd-req 0x00 --sd-id 0x00 --softdevice $(BUILD)/stripped_sd.hex $(BUILD)/stripped_sd.zip
	$(Q)nrfutil dfu usb-serial -pkg $(BUILD)/stripped_sd.zip -p $(NRFUTIL_PORT) -t 0

nrfutil_dfu_deploy: $(BUILD)/$(OUTPUT_FILENAME).hex
	$(Q)nrfutil pkg generate --hw-version 52 --sd-req $(NRFUTIL_SD_REQ) --application-version 1 --application $(BUILD)/$(OUTPUT_FILENAME).hex $(BUILD)/$(OUTPUT_FILENAME)_dfu.zip
	$(Q)nrfutil dfu usb-serial -pkg $(BUILD)/$(OUTPUT_FILENAME)_dfu.zip -p $(NRFUTIL_PORT) -t 0

deploy: nrfutil_dfu_deploy

else ifeq ($(FLASHER), bossac)

BOSSAC_PORT ?= /dev/ttyACM0
BOSSAC_OFFSET ?= 0x16000

deploy: $(BUILD)/$(OUTPUT_FILENAME).bin
	$(Q)bossac -e -w --offset=$(BOSSAC_OFFSET) --port=$(BOSSAC_PORT) -i -d -U -R $<

endif

flash: deploy

$(BUILD)/$(OUTPUT_FILENAME).elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_SHARED_C) $(DRIVERS_SRC_C) $(SRC_BOARD_MODULES) $(GEN_PINS_SRC)

# Making OBJ use an order-only dependency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(HEADER_BUILD)/pins.h

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_gen.c and pins.h
$(BUILD)/%_gen.c $(HEADER_BUILD)/%.h $(HEADER_BUILD)/%_af_const.h: $(BOARD_DIR)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board-csv $(BOARD_PINS) --af-csv $(AF_FILE) --prefix $(PREFIX_FILE) \
	    --output-source $(GEN_PINS_SRC) --output-header $(GEN_PINS_HDR) --output-af-const $(GEN_PINS_AF_CONST)

$(PY_BUILD)/nlr%.o: CFLAGS += -Os -fno-lto

include ../../py/mkrules.mk

