# Select the board to build for: if not given on the command line,
# then default to PYBV10.
BOARD ?= arduino_zero
ifeq ($(wildcard boards/$(BOARD)/.),)
$(error Invalid BOARD specified)
endif


# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

include ../py/mkenv.mk
-include mpconfigport.mk
include boards/$(BOARD)/mpconfigboard.mk

# qstr definitions (must come before including py.mk)
# TODO(tannewt): Support friendly pin names like the stmhal implementations do.
# Add $(BUILD)/pins_qstr.h $(BUILD)/modstm_qstr.h
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

BOSSAC := tools/bossac_osx

HAL_DIR=hal/$(MCU_SERIES)

INC += -I.
INC += -I..
INC += -I../lib/mp-readline
INC += -I../lib/timeutils
INC += -Iasf_conf/
INC += -Iasf/common/boards/
INC += -Iasf/common/services/sleepmgr/
INC += -Iasf/common/services/storage/ctrl_access/
INC += -Iasf/common/services/usb/
INC += -Iasf/common/services/usb/class/cdc/
INC += -Iasf/common/services/usb/class/cdc/device/
INC += -Iasf/common/services/usb/class/hid/
INC += -Iasf/common/services/usb/class/hid/device/
INC += -Iasf/common/services/usb/class/hid/device/kbd/
INC += -Iasf/common/services/usb/class/hid/device/mouse/
INC += -Iasf/common/services/usb/class/msc/
INC += -Iasf/common/services/usb/class/msc/device/
INC += -Iasf/common/services/usb/udc/
INC += -Iasf/common/utils
INC += -Iasf/common2/services/delay/
INC +=  $(addprefix -Iasf/sam0/,\
	drivers/events \
	drivers/extint \
	drivers/port \
	drivers/system \
	drivers/adc/adc_sam_d_r \
	drivers/dac \
	drivers/dac/dac_sam_d_c \
	drivers/sercom \
	drivers/sercom/i2c \
	drivers/system/clock \
	drivers/system/clock/clock_samd21_r21_da \
	drivers/system/interrupt \
	drivers/system/interrupt/system_interrupt_samd21 \
	drivers/system/pinmux \
	drivers/system/power/power_sam_d_r \
	drivers/system/reset/reset_sam_d_r \
	drivers/tc \
	drivers/usb \
	utils \
	utils/cmsis/samd21/include \
	utils/cmsis/samd21/source \
	utils/header_files \
	utils/preprocessor \
	)
INC += -Iasf/thirdparty/CMSIS/Include
INC += -Iboards
INC += -Iboards/$(BOARD)/
INC += -Ifreetouch/
INC += -I$(BUILD)

CFLAGS_CORTEX_M0 = \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m0plus \
	-msoft-float \
	-mfloat-abi=soft \
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
	-D__$(CHIP_VARIANT)__ \
	-DUSB_DEVICE_PRODUCT_ID=$(USB_PID) \
	-DUSB_DEVICE_VENDOR_ID=$(USB_VID) \
	-DBOARD=USER_BOARD \
	-ffunction-sections \
	-fdata-sections \
	-fshort-enums \
	-D ARM_MATH_CM0PLUS=true \
	-DSYSTICK_MODE \
	-DEXTINT_CALLBACK_MODE=true \
	-DUDD_ENABLE \
	-DUSART_CALLBACK_MODE=false \
	-DSPI_CALLBACK_MODE=false \
	-DI2C_MASTER_CALLBACK_MODE=false \
	-DDAC_CALLBACK_MODE=false \
	-DTCC_ASYNC=false \
	-DADC_CALLBACK_MODE=false \
	-DEVENTS_INTERRUPT_HOOKS_MODE=false \
	-DTC_ASYNC=true \
	-DUSB_DEVICE_LPM_SUPPORT \
	-DCIRCUITPY_CANARY_WORD=0xADAF00 \
	-DCIRCUITPY_SAFE_RESTART_WORD=0xDEADBEEF \
	--param max-inline-insns-single=500
CFLAGS = $(INC) -Wall -Werror -std=gnu11 -nostdlib $(CFLAGS_CORTEX_M0) $(COPT)

#Debugging/Optimization
# TODO(tannewt): Figure out what NDEBUG does. Adding it to the debug build
# reduces code size pretty dramatically.
ifeq ($(DEBUG), 1)
CFLAGS += -Os -ggdb -DNDEBUG -DENABLE_MICRO_TRACE_BUFFER
else
CFLAGS += -Os -DNDEBUG -flto
endif

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

#LIBM_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libm.a)
LDFLAGS = $(CFLAGS) -nostartfiles -fshort-enums -mthumb -mcpu=cortex-m0plus -Lasf/thirdparty/CMSIS/Lib/GCC/ -Wl,-nostdlib -Wl,-T,$(LD_FILE) -Wl,-Map=$@.map -Wl,-cref -Wl,-gc-sections -specs=nano.specs
LIBS = -larm_cortexM0l_math -lm -lgcc -lc

SRC_ASF = $(addprefix asf/sam0/,\
	drivers/adc/adc_sam_d_r/adc.c \
	drivers/dac/dac_sam_d_c/dac.c \
	drivers/dma/dma.c \
	drivers/events/events_sam_d_r/events.c \
	drivers/extint/extint_callback.c \
	drivers/extint/extint_sam_d_r/extint.c \
	drivers/nvm/nvm.c \
	drivers/port/port.c \
	drivers/sercom/i2c/i2c_sam0/i2c_master.c \
	drivers/sercom/sercom.c \
	drivers/sercom/sercom_interrupt.c \
	drivers/sercom/spi/spi.c \
	drivers/sercom/usart/usart.c \
	drivers/system/clock/clock_samd21_r21_da/clock.c \
	drivers/system/clock/clock_samd21_r21_da/gclk.c \
	drivers/system/interrupt/system_interrupt.c   \
	drivers/system/pinmux/pinmux.c \
	drivers/system/system.c \
	drivers/tc/tc_interrupt.c \
	drivers/tc/tc_sam_d_r/tc.c \
	drivers/tcc/tcc.c \
	drivers/usb/stack_interface/usb_device_udd.c \
	drivers/usb/stack_interface/usb_dual.c \
	drivers/usb/usb_sam_d_r/usb.c \
	)

SRC_C = \
	access_vfs.c \
	autoreload.c \
	background.c \
	builtin_open.c \
	fatfs_port.c \
	flash_api.c \
	main.c \
	moduos.c \
	mphalport.c \
	samd21_pins.c \
	shared_dma.c \
	rgb_led_status.c \
	tick.c \
	$(FLASH_IMPL) \
	bindings/samd/__init__.c \
	asf/common/services/sleepmgr/samd/sleepmgr.c \
	asf/common/services/storage/ctrl_access/ctrl_access.c  \
	asf/common/services/usb/class/cdc/device/udi_cdc.c \
	asf/common/services/usb/class/composite/device/udi_composite_desc.c \
	asf/common/services/usb/class/hid/device/udi_hid.c \
	asf/common/services/usb/class/hid/device/mouse/udi_hid_mouse.c \
	asf/common/services/usb/class/hid/device/kbd/udi_hid_kbd.c \
	asf/common/services/usb/class/msc/device/udi_msc.c     \
	asf/common/services/usb/udc/udc.c \
	asf/common/utils/interrupt/interrupt_sam_nvic.c \
	asf/common2/services/delay/sam0/systick_counter.c \
	asf/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c \
	asf/sam0/utils/cmsis/samd21/source/system_samd21.c \
	asf/sam0/utils/syscalls/gcc/syscalls.c \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	freetouch/adafruit_ptc.c \
	lib/fatfs/ff.c \
	lib/fatfs/option/ccsbcs.c \
	lib/timeutils/timeutils.c \
	lib/utils/buffer_helper.c \
	lib/utils/context_manager_helpers.c \
	lib/utils/interrupt_char.c \
	lib/utils/pyexec.c \
	lib/utils/pyhelp.c \
	lib/utils/stdout_helpers.c \
	lib/libc/string0.c \
	lib/mp-readline/builtin_input.c \
	lib/mp-readline/readline.c

STM_SRC_C = $(addprefix stmhal/,\
	pybstdio.c \
	)

SRC_COMMON_HAL = \
	board/__init__.c \
	microcontroller/__init__.c \
	microcontroller/Pin.c \
	analogio/__init__.c \
	analogio/AnalogIn.c \
	analogio/AnalogOut.c \
	audioio/__init__.c \
	audioio/AudioOut.c \
	digitalio/__init__.c \
	digitalio/DigitalInOut.c \
	pulseio/__init__.c \
	pulseio/PulseIn.c \
	pulseio/PulseOut.c \
	pulseio/PWMOut.c \
	busio/__init__.c \
	busio/I2C.c \
	busio/SPI.c \
	busio/UART.c \
	neopixel_write/__init__.c \
	time/__init__.c \
	touchio/__init__.c \
	touchio/TouchIn.c \
	usb_hid/__init__.c \
	usb_hid/Device.c

# These don't have corresponding files in each port but are still located in
# shared-bindings to make it clear what the contents of the modules are.
SRC_BINDINGS_ENUMS = \
	digitalio/Direction.c \
	digitalio/DriveMode.c \
	digitalio/Pull.c

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE = \
	help.c \
	bitbangio/__init__.c \
	bitbangio/I2C.c \
	bitbangio/OneWire.c \
	bitbangio/SPI.c \
	busio/OneWire.c \
	uheap/__init__.c \

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE))

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_ASF:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(STM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_EXPANDED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED_MODULE_EXPANDED:.c=.o))

SRC_QSTR += $(SRC_C) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED) $(STM_SRC_C)

all: $(BUILD)/firmware.bin $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ)
	$(STEPECHO) "LINK $@"
	$(Q)$(CC) -o $@ $(LDFLAGS) $^ -Wl,--start-group $(LIBS) -Wl,--end-group
	$(Q)$(SIZE) $@ | python3 ../tools/build_memory_info.py $(LD_FILE)

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .vectors -j .text -j .data $^ $@

$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	python2 ../tools/uf2/utils/uf2conv.py -c -o $@ $^

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

include ../py/mkrules.mk
