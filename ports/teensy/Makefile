include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(GEN_PINS_QSTR)

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

USE_FROZEN = 1
USE_MEMZIP = 0

ifeq ($(USE_MEMZIP),1)
SRC_C += \
	shared/memzip/import.c \
	shared/memzip/lexermemzip.c \
	shared/memzip/memzip.c \

OBJ += $(BUILD)/memzip-files.o

MAKE_MEMZIP = $(TOP)/shared/memzip/make-memzip.py
ifeq ($(MEMZIP_DIR),)
MEMZIP_DIR = memzip_files
endif

$(BUILD)/memzip-files.o: $(BUILD)/memzip-files.c
	$(call compile_c)

$(BUILD)/memzip-files.c: $(shell find ${MEMZIP_DIR} -type f)
	@$(ECHO) "Creating $@"
	$(Q)$(PYTHON) $(MAKE_MEMZIP) --zip-file $(BUILD)/memzip-files.zip --c-file $@ $(MEMZIP_DIR)

endif # USE_MEMZIP

ifeq ($(USE_FROZEN),1)

FROZEN_MANIFEST ?= "manifest.py"

CFLAGS += -DMICROPY_MODULE_FROZEN_STR

SRC_C += \
	lexerfrozen.c

endif # USE_FROZEN

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

# If you set USE_ARDUINO_TOOLCHAIN=1 then this makefile will attempt to use
# the toolchain that comes with Teensyduino
ifeq ($(USE_ARDUINO_TOOLCHAIN),)
USE_ARDUINO_TOOLCHAIN = 0
endif

ifeq ($(USE_ARDUINO_TOOLCHAIN),1)
ifeq ($(ARDUINO),)
$(error USE_ARDUINO_TOOLCHAIN requires that ARDUINO be set)
endif
endif

ifeq ($(USE_ARDUINO_TOOLCHAIN),1)
$(info Using ARDUINO toolchain)
CROSS_COMPILE ?= $(ARDUINO)/hardware/tools/arm-none-eabi/bin/arm-none-eabi-
else
$(info Using toolchain from PATH)
CROSS_COMPILE ?= arm-none-eabi-
endif

MAKE_PINS = make-pins.py
BOARD_PINS = teensy_pins.csv
AF_FILE = mk20dx256_af.csv
PREFIX_FILE = mk20dx256_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_gen.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py

CFLAGS_TEENSY = -DF_CPU=96000000 -DUSB_SERIAL -D__MK20DX256__
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -msoft-float -mfloat-abi=soft -fsingle-precision-constant -Wdouble-promotion $(CFLAGS_TEENSY)

INC += -I.
INC += -I$(TOP)
INC += -I$(TOP)/ports/stm32
INC += -I$(BUILD)
INC += -Icore

CFLAGS += $(INC) -Wall -Wpointer-arith -Werror -std=c99 -nostdlib $(CFLAGS_CORTEX_M4)
LDFLAGS += -nostdlib -T mk20dx256.ld -msoft-float -mfloat-abi=soft

ifeq ($(USE_ARDUINO_TOOLCHAIN),1)

LIBGCC_FILE_NAME = $(ARDUINO)/hardware/tools/arm-none-eabi/lib/gcc/arm-none-eabi/4.7.2/thumb2/libgcc.a
LIBM_FILE_NAME   = $(ARDUINO)/hardware/tools/arm-none-eabi/arm-none-eabi/lib/thumb2/libm.a
LIBC_FILE_NAME   = $(ARDUINO)/hardware/tools/arm-none-eabi/arm-none-eabi/lib/thumb2/libc.a

else

LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
LIBM_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libm.a)
LIBC_FILE_NAME   = $(shell $(CC) $(CFLAGS) -print-file-name=libc.a)

endif

#$(info %%%%% LIBGCC_FILE_NAME = $(LIBGCC_FILE_NAME))
#$(info %%%%% LIBM_FILE_NAME = $(LIBM_FILE_NAME))
#$(info %%%%% LIBC_FILE_NAME = $(LIBC_FILE_NAME))

#$(info %%%%% dirname LIBGCC_FILE_NAME = $(dir $(LIBGCC_FILE_NAME)))
#$(info %%%%% dirname LIBM_FILE_NAME = $(dir $(LIBM_FILE_NAME)))
#$(info %%%%% dirname LIBC_FILE_NAME = $(dir $(LIBC_FILE_NAME)))

LIBS  = -L $(dir $(LIBM_FILE_NAME)) -lm
LIBS += -L $(dir $(LIBC_FILE_NAME)) -lc
LIBS += -L $(dir $(LIBGCC_FILE_NAME)) -lgcc

#Debugging/Optimization
CFLAGS += -g  # always include debug info in the ELF
ifdef DEBUG
CFLAGS += -Og
else
CFLAGS += -Os #-DNDEBUG
endif
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += -Wl,--gc-sections

SRC_C += \
	hal_ftm.c \
	hal_gpio.c \
	help.c \
	main.c \
	lcd.c \
	led.c \
	modpyb.c \
	pin_defs_teensy.c \
	reg.c \
	teensy_hal.c \
	timer.c \
	uart.c \
	usb.c \

STM_SRC_C = $(addprefix ports/stm32/,\
	irq.c \
	pin.c \
	pin_named_pins.c \
	)

SHARED_SRC_C = $(addprefix shared/,\
	libc/string0.c \
	readline/readline.c \
	runtime/gchelper_native.c \
	runtime/pyexec.c \
	runtime/sys_stdio_mphal.c \
	)

SRC_TEENSY = $(addprefix core/,\
	mk20dx128.c \
	pins_teensy.c \
	analog.c \
	usb_desc.c \
	usb_dev.c \
	usb_mem.c \
	usb_serial.c \
	yield.c \
	)

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(STM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_TEENSY:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(BUILD)/shared/runtime/gchelper_thumb2.o
OBJ += $(GEN_PINS_SRC:.c=.o)

all: hex
hex: $(BUILD)/micropython.hex

ifeq ($(ARDUINO),)
post_compile: $(BUILD)/micropython.hex
	$(ECHO) "Please define ARDUINO (where TeensyDuino is installed)"
	exit 1

reboot:
	$(ECHO) "Please define ARDUINO (where TeensyDuino is installed)"
	exit 1

else
TOOLS_PATH = $(ARDUINO)/hardware/tools

post_compile: $(BUILD)/micropython.hex
	$(ECHO) "Preparing $@ for upload"
	$(Q)$(TOOLS_PATH)/teensy_post_compile -file="$(basename $(<F))" -path="$(abspath $(<D))" -tools="$(TOOLS_PATH)"

reboot:
	$(ECHO) "REBOOT"
	-$(Q)$(TOOLS_PATH)/teensy_reboot
endif

.PHONY: deploy
deploy: post_compile reboot

$(BUILD)/micropython.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o "$@" -Wl,-Map,$(@:.elf=.map) $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(ECHO) "HEX $<"
	$(Q)$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(STM_SRC_C) $(SHARED_SRC_C)
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
$(BUILD)/%_gen.c $(HEADER_BUILD)/%.h $(HEADER_BUILD)/%_af_const.h $(BUILD)/%_qstr.h: teensy_%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) --af-const $(GEN_PINS_AF_CONST) --af-py $(GEN_PINS_AF_PY) > $(GEN_PINS_SRC)

$(GEN_PINS_SRC:.c=.o): $(GEN_PINS_SRC)
	$(call compile_c)

$(BUILD)/%.pp: $(BUILD)/%.c
	$(ECHO) "PreProcess $<"
	$(Q)$(CC) $(CFLAGS) -E -Wp,-C,-dD,-dI -o $@ $<

include $(TOP)/py/mkrules.mk
