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
INC += -Icommon-hal/modules/
INC += -Iasf/common/boards/
INC += -Iasf/common/services/sleepmgr/
INC += -Iasf/common/services/storage/ctrl_access/
INC += -Iasf/common/services/usb/
INC += -Iasf/common/services/usb/class/cdc/
INC += -Iasf/common/services/usb/class/cdc/device/
INC += -Iasf/common/services/usb/class/msc/
INC += -Iasf/common/services/usb/class/msc/device/
INC += -Iasf/common/services/usb/udc/
INC += -Iasf/common/utils
INC += -Iasf/common2/services/delay/
INC +=  $(addprefix -Iasf/sam0/,\
	drivers/extint/ \
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
INC += -Iboards/$(BOARD)/
INC += -I$(BUILD)

CFLAGS_CORTEX_M0 = \
	-mthumb \
	-mabi=aapcs-linux \
	-mcpu=cortex-m0plus \
	-msoft-float \
	-mfloat-abi=soft \
	-fsingle-precision-constant \
	-Wdouble-promotion \
	-D__SAMD21G18A__ \
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
	-DUSART_CALLBACK_MODE=true \
	-DUSB_DEVICE_LPM_SUPPORT
CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M0) $(COPT)

#Debugging/Optimization
# TODO(tannewt): Figure out what NDEBUG does. Adding it to the debug build
# reduces code size pretty dramatically.
ifeq ($(DEBUG), 1)
CFLAGS += -Os -ggdb -DNDEBUG -DENABLE_MICRO_TRACE_BUFFER
else
CFLAGS += -Os -DNDEBUG
endif

LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
LIBM_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libm.a)
LDFLAGS = -Lasf/thirdparty/CMSIS/Lib/GCC/ -L $(dir $(LIBGCC_FILE_NAME)) -L $(dir $(LIBM_FILE_NAME)) -nostdlib -T $(LD_FILE) -Map=$@.map --cref --gc-sections
LIBS = -larm_cortexM0l_math -lm -lgcc -lc


SRC_ASF = $(addprefix asf/sam0/,\
	drivers/adc/adc_sam_d_r/adc.c \
	drivers/dac/dac_sam_d_c/dac.c \
	drivers/nvm/nvm.c \
	drivers/port/port.c \
	drivers/sercom/i2c/i2c_sam0/i2c_master.c \
	drivers/sercom/sercom.c \
	drivers/sercom/sercom_interrupt.c \
	drivers/sercom/usart/usart.c \
	drivers/sercom/usart/usart_interrupt.c \
	drivers/system/clock/clock_samd21_r21_da/clock.c \
	drivers/system/clock/clock_samd21_r21_da/gclk.c \
	drivers/system/interrupt/system_interrupt.c   \
	drivers/system/pinmux/pinmux.c \
	drivers/system/system.c \
	drivers/tc/tc_sam_d_r/tc.c \
	drivers/tcc/tcc.c \
	drivers/usb/stack_interface/usb_device_udd.c \
	drivers/usb/stack_interface/usb_dual.c \
	drivers/usb/usb_sam_d_r/usb.c \
	)

SRC_C = \
	builtin_open.c \
	fatfs_port.c \
	main.c \
	modmachine.c \
	modmachine_adc.c \
	modmachine_dac.c \
	modmachine_pin.c \
	modmachine_pwm.c \
	modneopixel_write.c \
	moduos.c \
	modutime.c \
	mphalport.c \
	pin_named_pins.c \
	rom_fs.c \
	samdneopixel.c \
	storage.c \
	uart.c \
	asf/common/services/sleepmgr/samd/sleepmgr.c \
	asf/common/services/storage/ctrl_access/ctrl_access.c  \
	asf/common/services/usb/class/cdc/device/udi_cdc.c \
	asf/common/services/usb/class/composite/device/udi_composite_desc.c \
	asf/common/services/usb/class/msc/device/udi_msc.c     \
	asf/common/services/usb/udc/udc.c \
	asf/common/utils/interrupt/interrupt_sam_nvic.c \
	asf/common2/services/delay/sam0/systick_counter.c \
	asf/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c \
	asf/sam0/utils/cmsis/samd21/source/system_samd21.c \
	asf/sam0/utils/syscalls/gcc/syscalls.c \
	boards/$(BOARD)/init.c \
	boards/$(BOARD)/pins.c \
	lib/fatfs/ff.c \
	lib/fatfs/option/ccsbcs.c \
	lib/timeutils/timeutils.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c

STM_SRC_C = $(addprefix stmhal/,\
	pybstdio.c \
	input.c \
	)

# TODO(tannewt): Use this sed line to extract the RST docs from these sources:
#   sed': sed -n 's+^//|++p' ../api/machine.c
#
# RST lines are prefixed with //|
SRC_BINDINGS = \
	modules/machine.c

SRC_BINDINGS_EXPANDED = $(addprefix shared-bindings/, $(SRC_BINDINGS)) \
                        $(addprefix common-hal/, $(SRC_BINDINGS))

SRC_AUTOGEN = \
 $(BUILD)/_frozen_mpy.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_ASF:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_AUTOGEN:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(STM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_BINDINGS_EXPANDED:.c=.o))

SRC_QSTR += $(SRC_C) $(SRC_BINDINGS_EXPANDED) $(STM_SRC_C)

all: $(BUILD)/firmware.bin

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)../tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .vectors -j .text -j .data $^ $@

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
