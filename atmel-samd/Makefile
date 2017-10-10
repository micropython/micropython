# Select the board to build for: if not given on the command line,
# then default to PYBV10.
BOARD ?= metro_m0_express
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif


# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

include ../py/mkenv.mk
-include mpconfigport.mk
include boards/$(BOARD)/mpconfigboard.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

include $(TOP)/supervisor/supervisor.mk

CROSS_COMPILE = arm-none-eabi-

BOSSAC := tools/bossac_osx

HAL_DIR=hal/$(MCU_SERIES)

INC += -I. \
       -I.. \
       -I../lib/mp-readline \
       -I../lib/timeutils \
       -Iasf4/$(CHIP_FAMILY) \
       -Iasf4/$(CHIP_FAMILY)/hal/include \
       -Iasf4/$(CHIP_FAMILY)/hal/utils/include \
       -Iasf4/$(CHIP_FAMILY)/hri \
       -Iasf4/$(CHIP_FAMILY)/hpl/core \
       -Iasf4/$(CHIP_FAMILY)/hpl/pm \
       -Iasf4/$(CHIP_FAMILY)/hpl/port \
       -Iasf4/$(CHIP_FAMILY)/hpl/tc \
       -Iasf4/$(CHIP_FAMILY)/include \
       -Iasf4/$(CHIP_FAMILY)/CMSIS/Include \
       -Iasf4/$(CHIP_FAMILY)/usb \
       -Iasf4/$(CHIP_FAMILY)/usb/class/cdc \
       -Iasf4/$(CHIP_FAMILY)/usb/class/hid \
       -Iasf4/$(CHIP_FAMILY)/usb/device \
       -Iasf4_conf/$(CHIP_FAMILY) \
       -Iboards/$(BOARD) \
       -Iboards/ \
       -Ifreetouch \
       -I$(BUILD)

BASE_CFLAGS = \
	-fsingle-precision-constant \
	-fno-strict-aliasing \
	-Wdouble-promotion \
	-Wno-endif-labels \
	-Wstrict-prototypes \
	-Werror-implicit-function-declaration \
	-Wpointer-arith \
	-Wfloat-equal \
	-Wundef \
	-Wshadow \
	-Wwrite-strings \
	-Wsign-compare \
	-Wmissing-format-attribute \
	-Wno-deprecated-declarations \
	-Wpacked \
	-Wnested-externs \
	-Wunreachable-code \
	-Wcast-align \
	-Wno-error=lto-type-mismatch \
	-D__$(CHIP_VARIANT)__ \
	-DCONF_USB_COMPOSITE_IDPRODUCT=$(USB_PID) \
	-DCONF_USB_COMPOSITE_IDVENDER=$(USB_VID) \
	-ffunction-sections \
	-fdata-sections \
	-fshort-enums \
	--param max-inline-insns-single=500

# NDEBUG disables assert() statements. This reduces code size pretty dramatically, per tannewt.
ifeq ($(CHIP_FAMILY), samd21)
CFLAGS = -Os -DNDEBUG -finline-limit=49
endif
ifeq ($(CHIP_FAMILY), samd51)
CFLAGS = -O2 -DNDEBUG
endif

#Debugging/Optimization
ifeq ($(DEBUG), 1)
# Turn on Python modules useful for debugging (e.g. uheap, ustack).
CFLAGS += -ggdb
ifeq ($(CHIP_FAMILY), samd21)
CFLAGS += -DNDEBUG -DENABLE_MICRO_TRACE_BUFFER
endif
else
# -finline-limit can shrink the image size. -finline-limit=80 or so is similar to not having it on.
# There is no simple default value, though.
CFLAGS += -DNDEBUG -flto -finline-limit=49
endif

CFLAGS += $(INC) -Wall -Werror -std=gnu11 -nostdlib $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT)

ifeq ($(CHIP_FAMILY), samd21)
CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m0plus \
	-msoft-float \
	-mfloat-abi=soft \
	-DSAMD21
endif
ifeq ($(CHIP_FAMILY), samd51)
CFLAGS += \
	-mthumb \
	-mabi=aapcs-linux \
	-mlong-calls \
	-mtune=cortex-m4 \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-DSAMD51
endif

ifneq ($(FROZEN_DIR),)
# To use frozen source modules, put your .py files in a subdirectory (eg scripts/)
# and then invoke make with FROZEN_DIR=scripts (be sure to build from scratch).
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
CFLAGS += -Wno-error=lto-type-mismatch
endif

# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen or FROZEN_MPY_DIRS="dir1 dir2"
# (be sure to build from scratch).

ifneq ($(FROZEN_MPY_DIRS),)
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -Wno-error=lto-type-mismatch
endif

#LIBM_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libm.a)
LDFLAGS = $(CFLAGS) -nostartfiles -fshort-enums -Wl,-nostdlib -Wl,-T,$(LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LIBS := -lgcc -lc


ifeq ($(CHIP_FAMILY), samd21)
LDFLAGS += -mthumb -mcpu=cortex-m0plus -Lasf/thirdparty/CMSIS/Lib/GCC/
LIBS :=  -lm $(LIBS) # -larm_cortexM0l_math
else ifeq ($(CHIP_FAMILY), samd51)
LDFLAGS += -mthumb -mcpu=cortex-m4
LIBS := -lm $(LIBS)
endif

SRC_ASF := \
	gcc/gcc/startup_$(CHIP_FAMILY).c \
	gcc/system_$(CHIP_FAMILY).c \
	hal/src/hal_atomic.c \
	hal/src/hal_delay.c \
	hal/src/hal_flash.c \
	hal/src/hal_sleep.c \
	hal/src/hal_timer.c \
	hal/src/hal_usb_device.c \
	hpl/core/hpl_init.c \
	hpl/gclk/hpl_gclk.c \
	hpl/nvmctrl/hpl_nvmctrl.c \
	hpl/pm/hpl_pm.c \
	hpl/rtc/hpl_rtc.c \
	hpl/systick/hpl_systick.c \
	hpl/tc/hpl_tc.c \
	hpl/usb/hpl_usb.c \
	usb/class/cdc/device/cdcdf_acm.c \
	usb/device/usbdc.c \
	usb/usb_protocol.c \
	hal/utils/src/utils_list.c \

ifeq ($(CHIP_FAMILY), samd21)
SRC_ASF += \
	hpl/core/hpl_core_m0plus_base.c \
	hpl/sysctrl/hpl_sysctrl.c \

else ifeq ($(CHIP_FAMILY), samd51)
SRC_ASF += \
    hal/src/hal_rand_sync.c \
	hpl/core/hpl_core_m4.c \
	hpl/mclk/hpl_mclk.c \
	hpl/osc32kctrl/hpl_osc32kctrl.c \
	hpl/oscctrl/hpl_oscctrl.c \
	hpl/trng/hpl_trng.c \

endif

SRC_ASF := $(addprefix asf4/$(CHIP_FAMILY)/, $(SRC_ASF))

# Skip this source for now.
#	access_vfs.c \
	shared_dma.c \

SRC_C = \
	background.c \
	fatfs_port.c \
	flash_api.c \
	mphalport.c \
	reset.c \
	$(CHIP_FAMILY)_pins.c \
	tick.c \
	usb.c \
	$(FLASH_IMPL) \
	bindings/samd/__init__.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	lib/oofatfs/ff.c \
	lib/oofatfs/option/ccsbcs.c \
	lib/timeutils/timeutils.c \
	lib/utils/buffer_helper.c \
	lib/utils/context_manager_helpers.c \
	lib/utils/interrupt_char.c \
	lib/utils/pyexec.c \
	lib/utils/stdout_helpers.c \
	lib/utils/sys_stdio_mphal.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	# freetouch/adafruit_ptc.c \

SRC_COMMON_HAL = \
	board/__init__.c \
	digitalio/__init__.c \
	digitalio/DigitalInOut.c \
	microcontroller/__init__.c \
	microcontroller/Pin.c \
	microcontroller/Processor.c \
	os/__init__.c \
	time/__init__.c
#	analogio/__init__.c \
	analogio/AnalogIn.c \
	analogio/AnalogOut.c \
	audiobusio/__init__.c \
	audiobusio/PDMIn.c \
	audioio/__init__.c \
	audioio/AudioOut.c \
	busio/__init__.c \
	busio/I2C.c \
	busio/SPI.c \
	busio/UART.c \
	neopixel_write/__init__.c \
	nvm/__init__.c \
	nvm/ByteArray.c \
	pulseio/__init__.c \
	pulseio/PulseIn.c \
	pulseio/PulseOut.c \
	pulseio/PWMOut.c \
	storage/__init__.c \
	touchio/__init__.c \
	touchio/TouchIn.c \
	usb_hid/__init__.c \
	usb_hid/Device.c

# These don't have corresponding files in each port but are still located in
# shared-bindings to make it clear what the contents of the modules are.
SRC_BINDINGS_ENUMS = \
	digitalio/Direction.c \
	digitalio/DriveMode.c \
	digitalio/Pull.c \
	help.c

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE = \
	bitbangio/__init__.c \
	bitbangio/I2C.c \
	bitbangio/OneWire.c \
	bitbangio/SPI.c \
	busio/OneWire.c \
	os/__init__.c \
	random/__init__.c \
	storage/__init__.c \
	uheap/__init__.c \
	ustack/__init__.c

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE))

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_ASF:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))

SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED) $(STM_SRC_C)

all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | python3 $(TOP)/tools/build_memory_info.py $(LD_FILE)

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	python2 $(TOP)/tools/uf2/utils/uf2conv.py -c -o $@ $^

deploy: $(BUILD)/firmware.bin
	$(ECHO) "Writing $< to the board"
	$(BOSSAC) -u $<

# Run emulation build on a POSIX system with suitable terminal settings
run:
	stty raw opost -echo
	build/firmware.elf
	@echo Resetting terminal...
# This sleep is useful to spot segfaults
	sleep 1
	reset

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

include $(TOP)/py/mkrules.mk
