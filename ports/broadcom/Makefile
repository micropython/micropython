# Select the board to build for.
BOARD=raspberrypi_pi4

ifeq ($(BOARD),)
	$(error You must provide a BOARD parameter)
else
	ifeq ($(wildcard boards/$(BOARD)/.),)
		$(error Invalid BOARD specified)
	endif
endif

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

include ../../py/mkenv.mk
# Board-specific
include boards/$(BOARD)/mpconfigboard.mk
# Port-specific
include mpconfigport.mk
# CircuitPython-specific
include $(TOP)/py/circuitpy_mpconfig.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

include $(TOP)/supervisor/supervisor.mk

# Include make rules and variables common across CircuitPython builds.
include $(TOP)/py/circuitpy_defns.mk

HAL_DIR=hal/$(MCU_SERIES)

INC += -I. \
	   -I../.. \
	   -I../../lib/mp-readline \
	   -I../../lib/timeutils \
	   -I../../lib/sdmmc/include \
	   -Iboards/$(BOARD) \
	   -Iboards/ \
	   -Iperipherals/ \
       -I../../lib/tinyusb/src \
       -I../../supervisor/shared/usb \
	   -I$(BUILD)


SRC_C += bindings/videocore/__init__.c \
         bindings/videocore/Framebuffer.c \
		 boards/$(BOARD)/board.c \
	     boards/$(BOARD)/pins.c \
	     background.c \
	     common-hal/videocore/Framebuffer.c \
	     fatfs_port.c \
		 mphalport.c \
		 lib/sdmmc/sdmmc_cmd.c \
		 lib/sdmmc/sdmmc_common.c \
		 lib/sdmmc/sdmmc_init.c \
		 lib/sdmmc/sdmmc_io.c \
		 lib/sdmmc/sdmmc_mmc.c \
		 lib/sdmmc/sdmmc_sd.c \
		 lib/tinyusb/src/portable/synopsys/dwc2/dcd_dwc2.c \
		 peripherals/broadcom/caches.c \
		 peripherals/broadcom/gen/interrupt_handlers.c \
		 peripherals/broadcom/gen/pins.c \
		 peripherals/broadcom/gpio.c \
		 peripherals/broadcom/interrupts.c \
		 peripherals/broadcom/mmu.c \
		 peripherals/broadcom/vcmailbox.c


SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
						  $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
					      $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
														 $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
														 $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

# There may be duplicates between SRC_COMMON_HAL_EXPANDED and SRC_SHARED_MODULE_EXPANDED,
# because a few modules have files both in common-hal/ and shared-modules/.
# Doing a $(sort ...) removes duplicates as part of sorting.
SRC_COMMON_HAL_SHARED_MODULE_EXPANDED = $(sort $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED))
SRC_S = peripherals/broadcom/boot.s

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
	ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

# BCM CLFAGS
CFLAGS += -ffreestanding -nostartfiles -DMICROPY_HW_MCU_NAME="\"$(CHIP_VARIANT)\""

ifeq ($(CHIP_VARIANT), "bcm2711")
CFLAGS += -mcpu=cortex-a72 -DBCM_VERSION=2711
CROSS_COMPILE = aarch64-none-elf-
else
CFLAGS += -mcpu=cortex-a53 -DBCM_VERSION=2837
CROSS_COMPILE = aarch64-none-elf-
# TODO add 32-bit support for Cortex-A7 and ARM1176
endif


OPTIMIZATION_FLAGS ?= -O3
CFLAGS += $(OPTIMIZATION_FLAGS)

# TinyUSB defines
CFLAGS += -DCFG_TUSB_MCU=OPT_MCU_BCM2711 -DCFG_TUD_MIDI_RX_BUFSIZE=512 \
          -DCFG_TUD_CDC_RX_BUFSIZE=512 -DCFG_TUD_MIDI_TX_BUFSIZE=512 \
          -DCFG_TUD_CDC_TX_BUFSIZE=512 -DCFG_TUD_MSC_BUFSIZE=1024

#Debugging/Optimization
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb3 -Og
	# No LTO because we may place some functions in RAM instead of flash.
else
	CFLAGS += -DNDEBUG -ggdb3

	# No LTO because we may place some functions in RAM instead of flash.

	ifdef CFLAGS_BOARD
		CFLAGS += $(CFLAGS_BOARD)
	endif
endif


CFLAGS += $(INC) -Wall -Werror -std=gnu11 $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) $(DISABLE_WARNINGS)


SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED)

LDFLAGS += $(CFLAGS) -T peripherals/broadcom/link.ld -Wl,--gc-sections -Wl,-Map=$@.map # -Wl,--cref

# Use toolchain libm if we're not using our own.
ifndef INTERNAL_LIBM
LIBS += -lm
endif

all: $(BUILD)/firmware.kernel8.img $(BUILD)/firmware.disk.img.zip

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel8.elf: $(OBJ)
	$(STEPECHO) "LINK $@"
	$(Q)echo $(OBJ) > $(BUILD)/firmware.objs
	$(Q)$(CC) -o $@ $(LDFLAGS) @$(BUILD)/firmware.objs -Wl,--start-group $(LIBS) -Wl,--end-group

$(BUILD)/kernel8.img: $(BUILD)/kernel8.elf
	$(STEPECHO) "Create $@"
	$(OBJCOPY) -O binary $(BUILD)/kernel8.elf $@

$(BUILD)/firmware.kernel8.img: $(BUILD)/kernel8.img
	$(STEPECHO) "Create $@"
	$(CP) $^ $@

$(BUILD)/firmware.disk.img.zip: $(BUILD)/kernel8.img
	$(STEPECHO) "Create $@"
	$(Q)dd if=/dev/zero of=$(BUILD)/circuitpython-disk.img bs=1 count=0 seek=256M
	$(Q)parted -s $(BUILD)/circuitpython-disk.img mktable msdos
	$(Q)parted -s $(BUILD)/circuitpython-disk.img mkpart primary fat32 0% 100%
	$(Q)mkfs.fat -F 32 -n BOOT --offset=2048 $(BUILD)/circuitpython-disk.img

	$(Q)mcopy -i $(BUILD)/circuitpython-disk.img@@1M config.txt firmware/bootcode.bin firmware/fixup* firmware/start* ::
	$(Q)mcopy -i $(BUILD)/circuitpython-disk.img@@1M $(BUILD)/kernel8.img ::
	$(Q)zip $@ $(BUILD)/circuitpython-disk.img
	$(Q)rm $(BUILD)/circuitpython-disk.img

include $(TOP)/py/mkrules.mk


# Print out the value of a make variable.
# https://stackoverflow.com/questions/16467718/how-to-print-out-a-variable-in-makefile
print-%:
	@echo $* = $($*)
