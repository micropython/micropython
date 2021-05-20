BOARD ?= XIAO_M0
#BOARD ?= ADAFRUIT_ITSYBITSY_M4_EXPRESS
BOARD_DIR ?= boards/$(BOARD)
BUILD ?= build-$(BOARD)

CROSS_COMPILE ?= arm-none-eabi-
UF2CONV ?= $(TOP)/tools/uf2conv.py

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif


# Qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES = $(BOARD_DIR)/mpconfigboard.h

include ../../py/mkenv.mk
include $(BOARD_DIR)/mpconfigboard.mk

MCU_SERIES_LOWER = $(shell echo $(MCU_SERIES) | tr '[:upper:]' '[:lower:]')

####################################
FROZEN_MANIFEST ?= boards/manifest.py
FROZEN_DIR ?=
FROZEN_MPY_DIR ?=
###################################

# Include py core make definitions
include $(TOP)/py/py.mk

GIT_SUBMODULES = lib/asf4 lib/tinyusb

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(BOARD_DIR)
INC += -I$(TOP)/lib/cmsis/inc
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hal/include
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hal/utils/include
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/config
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hri
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hpl/core
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hpl/gclk
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hpl/pm
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hpl/port
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hpl/rtc
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/hpl/tc
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/include
INC += -I$(TOP)/lib/asf4/$(MCU_SERIES_LOWER)/include/pio
INC += -I$(TOP)/lib/tinyusb/src

CFLAGS_MCU_SAMD21 = -mtune=cortex-m0plus -mcpu=cortex-m0plus -msoft-float
CFLAGS_MCU_SAMD51 = -mtune=cortex-m4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS = $(INC) -Wall -Werror -std=c99 -nostdlib -mthumb $(CFLAGS_MCU_$(MCU_SERIES)) -fsingle-precision-constant -Wdouble-promotion
CFLAGS += -DMCU_$(MCU_SERIES) -D__$(CMSIS_MCU)__
CFLAGS += $(CFLAGS_MOD) $(CFLAGS_EXTRA)
LDFLAGS = -nostdlib $(addprefix -T,$(LD_FILES)) -Map=$@.map --cref
LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# Tune for Debugging or Optimization
ifeq ($(DEBUG),1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
LDFLAGS += --gc-sections
CFLAGS += -fdata-sections -ffunction-sections
endif

SRC_C = \
	main.c \
	help.c \
	moduos.c \
	modutime.c \
	modmachine.c \
        $(BOARD_DIR)/pins.c \
	machine_pin.c \
	machine_led.c \
	modsamd.c \
	samd_flash.c \
	mphalport.c \
	samd_isr.c \
	samd_soc.c \
	tusb_port.c \
	lib/libc/string0.c \
	lib/libm/ef_sqrt.c \
	lib/libm/fmodf.c \
	lib/libm/math.c \
	lib/libm/nearbyintf.c \
	lib/timeutils/timeutils.c \
	lib/mp-readline/readline.c \
	lib/tinyusb/src/class/cdc/cdc_device.c \
	lib/tinyusb/src/common/tusb_fifo.c \
	lib/tinyusb/src/device/usbd.c \
	lib/tinyusb/src/device/usbd_control.c \
	lib/tinyusb/src/portable/microchip/samd/dcd_samd.c \
	lib/tinyusb/src/tusb.c \
	lib/utils/gchelper_native.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c \
	lib/utils/sys_stdio_mphal.c \
	lib/asf4/$(MCU_SERIES_LOWER)/hal/src/hal_flash.c \
	lib/asf4/$(MCU_SERIES_LOWER)/hal/src/hal_atomic.c \
	lib/asf4/$(MCU_SERIES_LOWER)/hpl/nvmctrl/hpl_nvmctrl.c \


#	samd_flash.c 

ifeq ($(MCU_SERIES),SAMD21)
SRC_S = lib/utils/gchelper_m0.s
else
SRC_S = lib/utils/gchelper_m3.s
endif

# List of sources for qstr extraction
SRC_QSTR += moduos.c \
            modutime.c \
            modmachine.c \
            machine_pin.c \
            machine_led.c \
            modsamd.c \
            samd_flash.c 


OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

######### Frozen #######
ifneq ($(FROZEN_MANIFEST)$(FROZEN_MPY_DIR),)
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
endif

ifneq ($(FROZEN_MANIFEST)$(FROZEN_DIR),)
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
endif
########################

# Workaround for bug in older gcc, warning on "static usbd_device_t _usbd_dev = { 0 };"
$(BUILD)/lib/tinyusb/src/device/usbd.o: CFLAGS += -Wno-missing-braces

all: $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin

$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(Q)$(PYTHON) $(UF2CONV) -b $(TEXT0) -c -o $@ $<

include $(TOP)/py/mkrules.mk
