include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

ifeq ($(ARDUINO),)
$(error Please define ARDUINO (where TeensyDuino is installed))
endif
TOOLS_PATH = $(ARDUINO)/hardware/tools
COMPILER_PATH = $(TOOLS_PATH)/arm-none-eabi/bin
CORE_PATH = $(ARDUINO)/hardware/teensy/cores/teensy3

CROSS_COMPILE = $(COMPILER_PATH)/arm-none-eabi-

CFLAGS_TEENSY = -DF_CPU=96000000 -DUSB_SERIAL -D__MK20DX256__
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -fsingle-precision-constant -Wdouble-promotion $(CFLAGS_TEENSY)

INC =  -I.
INC += -I$(PY_SRC)
INC += -I../stmhal
INC += -I$(BUILD)
INC += -I$(CORE_PATH)

CFLAGS = $(INC) -Wall -ansi -std=gnu99 $(CFLAGS_CORTEX_M4)
LDFLAGS = -nostdlib -T mk20dx256.ld
LIBS  = -L $(COMPILER_PATH)/../arm-none-eabi/lib/thumb2 -lm
LIBS += -L $(COMPILER_PATH)/../arm-none-eabi/lib/thumb2 -lc
LIBS += -L $(COMPILER_PATH)/../lib/gcc/arm-none-eabi/4.7.2/thumb2 -lgcc

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
	teensy_hal.c \
	uart.c \
	usb.c \

STM_SRC_C = $(addprefix stmhal/,\
	gccollect.c \
	input.c \
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

SRC_TEENSY = \
	mk20dx128.c \
	pins_teensy.c \
	analog.c \
	usb_desc.c \
	usb_dev.c \
	usb_mem.c \
	usb_serial.c \
	yield.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(STM_SRC_C:.c=.o) $(STM_SRC_S:.s=.o) $(SRC_TEENSY:.c=.o))
OBJ += $(BUILD)/pins_gen.o

all: hex
hex: $(BUILD)/micropython-mz.hex

post_compile: $(BUILD)/micropython-mz.hex
	$(ECHO) "Preparing $@ for upload"
	$(Q)$(TOOLS_PATH)/teensy_post_compile -file="$(basename $(<F))" -path="$(<D)" -tools="$(TOOLS_PATH)"

reboot:
	$(ECHO) "REBOOT"
	-$(Q)$(TOOLS_PATH)/teensy_reboot

upload: post_compile reboot

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

$(BUILD)/%.o: $(CORE_PATH)/%.c
	$(call compile_c)

MAKE_PINS = make-pins.py
BOARD_PINS = teensy-pins.csv
AF_FILE = mk20dx256-af.csv
PREFIX_FILE = mk20dx256-prefix.c
GEN_PINS_SRC = $(BUILD)/pins_gen.c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h

# Making OBJ use an order-only depenedency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
$(OBJ): | $(HEADER_BUILD)/pins.h

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
$(BUILD)/%_gen.c $(HEADER_BUILD)/%.h: teensy-%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE)
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) > $(GEN_PINS_SRC)

$(BUILD)/pins_gen.o: $(BUILD)/pins_gen.c
	$(call compile_c)

$(BUILD)/%.pp: $(BUILD)/%.c
	$(ECHO) "PreProcess $<"
	$(Q)$(CC) $(CFLAGS) -E -Wp,-C,-dD,-dI -o $@ $<

include ../py/mkrules.mk
