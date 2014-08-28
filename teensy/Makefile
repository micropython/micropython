include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h $(BUILD)/pins_qstr.h

# include py core make definitions
include ../py/py.mk

CROSS_COMPILE = arm-none-eabi-

CFLAGS_TEENSY = -DF_CPU=96000000 -DUSB_SERIAL -D__MK20DX256__
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -fsingle-precision-constant -Wdouble-promotion $(CFLAGS_TEENSY)

INC =  -I.
INC += -I$(PY_SRC)
INC += -I../stmhal
INC += -I$(BUILD)
INC += -Icore

CFLAGS = $(INC) -Wall -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4)
LDFLAGS = -nostdlib -T mk20dx256.ld

LIBGCC_FILE_NAME = $(shell $(CC) -print-libgcc-file-name)
LIBM_FILE_NAME   = $(shell $(CC) -print-file-name=libm.a)
LIBC_FILE_NAME   = $(shell $(CC) -print-file-name=libc.a)

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
ifdef DEBUG
CFLAGS += -Og -ggdb
else
CFLAGS += -Os #-DNDEBUG
endif

SRC_C = \
	hal_gpio.c \
	help.c \
	import.c \
	main.c \
	lcd.c \
	led.c \
	lexermemzip.c \
	memzip.c \
	modpyb.c \
	pin_defs_teensy.c \
	teensy_hal.c \
	uart.c \
	usb.c \

STM_SRC_C = $(addprefix stmhal/,\
	gccollect.c \
	input.c \
	irq.c \
	pin.c \
	pin_named_pins.c \
	printf.c \
	pyexec.c \
	pybstdio.c \
	readline.c \
	string0.c \
	)

STM_SRC_S = $(addprefix stmhal/,\
	gchelper.s \
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

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(STM_SRC_C:.c=.o) $(STM_SRC_S:.s=.o) $(SRC_TEENSY:.c=.o))
OBJ += $(BUILD)/pins_gen.o

all: hex
hex: $(BUILD)/micropython-mz.hex

ifeq ($(ARDUINO),)
post_compile: $(BUILD)/micropython-mz.hex
	$(ECHO) "Please define ARDUINO (where TeensyDuino is installed)"
	exit 1

reboot:
	$(ECHO) "Please define ARDUINO (where TeensyDuino is installed)"
	exit 1

else
TOOLS_PATH = $(ARDUINO)/hardware/tools

post_compile: $(BUILD)/micropython-mz.hex
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
	$(Q)$(CC) $(LDFLAGS) -o "$@" -Wl,-Map,$(@:.elf=.map) $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

ifeq ($(MEMZIP_DIR),)
MEMZIP_DIR = memzip_files
endif

$(BUILD)/micropython-mz.hex: $(BUILD)/micropython.hex $(shell find ${MEMZIP_DIR} -type f)
	@$(ECHO) "Creating $@"
	$(Q)./add-memzip.sh $< $@ ${MEMZIP_DIR}

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(ECHO) "HEX $<"
	$(Q)$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

MAKE_PINS = make-pins.py
BOARD_PINS = teensy_pins.csv
AF_FILE = mk20dx256_af.csv
PREFIX_FILE = mk20dx256_prefix.c
GEN_PINS_SRC = $(BUILD)/pins_gen.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h

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
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) --af-const $(GEN_PINS_AF_CONST) > $(GEN_PINS_SRC)

$(BUILD)/pins_gen.o: $(BUILD)/pins_gen.c
	$(call compile_c)

$(BUILD)/%.pp: $(BUILD)/%.c
	$(ECHO) "PreProcess $<"
	$(Q)$(CC) $(CFLAGS) -E -Wp,-C,-dD,-dI -o $@ $<

include ../py/mkrules.mk
