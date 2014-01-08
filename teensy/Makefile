ifeq ($(ARDUINO),)
$(error Please define ARDUINO (where TeensyDuino is installed))
endif
TOOLS_PATH = $(ARDUINO)/hardware/tools
COMPILER_PATH = $(TOOLS_PATH)/arm-none-eabi/bin
CORE_PATH = $(ARDUINO)/hardware/teensy/cores/teensy3

PYSRC=../py
BUILD=build

AS = $(COMPILER_PATH)/arm-none-eabi-as
CC = $(COMPILER_PATH)/arm-none-eabi-gcc
LD = $(COMPILER_PATH)/arm-none-eabi-ld
OBJCOPY = $(COMPILER_PATH)/arm-none-eabi-objcopy
SIZE = $(COMPILER_PATH)/arm-none-eabi-size

CFLAGS_TEENSY = -DF_CPU=96000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -D__MK20DX256__
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -fsingle-precision-constant -Wdouble-promotion $(CFLAGS_TEENSY)
CFLAGS = -I. -I$(PYSRC) -I$(CORE_PATH) -Wall -ansi -std=gnu99 -Os -DNDEBUG $(CFLAGS_CORTEX_M4)
LDFLAGS = -nostdlib -T mk20dx256.ld
LIBS = -L $(COMPILER_PATH)/../lib/gcc/arm-none-eabi/4.7.2/thumb2 -lgcc

SRC_C = \
	main.c \
	lcd.c \
	led.c \
	lexerfatfs.c \
	usart.c \
	usb.c \

STM_SRC_C = \
	malloc0.c \
	printf.c \
	string0.c \

STM_SRC_S = \
	gchelper.s \

PY_O = \
	nlrthumb.o \
	gc.o \
	malloc.o \
	qstr.o \
	vstr.o \
	unicode.o \
	lexer.o \
	lexerstr.o \
	parse.o \
	scope.o \
	compile.o \
	emitcommon.o \
	emitpass1.o \
	emitbc.o \
	asmthumb.o \
	emitnthumb.o \
	emitinlinethumb.o \
	runtime.o \
	map.o \
	obj.o \
	objbool.o \
	objboundmeth.o \
	objcell.o \
	objclass.o \
	objclosure.o \
	objcomplex.o \
	objdict.o \
	objexcept.o \
	objfloat.o \
	objfun.o \
	objgenerator.o \
	objinstance.o \
	objint.o \
	objlist.o \
	objmodule.o \
	objnone.o \
	objrange.o \
	objset.o \
	objslice.o \
	objstr.o \
	objtuple.o \
	objtype.o \
	builtin.o \
	builtinimport.o \
	vm.o \
	showbc.o \
	repl.o \

SRC_TEENSY = \
	mk20dx128.c \
	pins_teensy.c \
	analog.c \
	usb_desc.c \
	usb_dev.c \
	usb_mem.c \
	usb_serial.c \
	yield.c \

STM_SRC_C_OBJ = $(STM_SRC_C:.c=.o)
STM_SRC_S_OBJ = $(STM_SRC_S:.s=.o)

OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(STM_SRC_C_OBJ) $(STM_SRC_S_OBJ) $(PY_O) $(SRC_TEENSY:.c=.o))
#LIB = -lreadline
# the following is needed for BSD
#LIB += -ltermcap
PROG = micropython

all: hex
hex: $(PROG).hex

post_compile: $(PROG).hex
	$(TOOLS_PATH)/teensy_post_compile -file="$(basename $<)" -path="$(CURDIR)" -tools="$(TOOLS_PATH)"

reboot:
	-$(TOOLS_PATH)/teensy_reboot

upload: post_compile reboot

$(PROG).elf: $(BUILD) $(OBJ)
	$(CC) $(LDFLAGS) -o "$@" -Wl,-Map,$(PROG).map $(OBJ) $(LIBS)

%.hex: %.elf
	$(SIZE) "$<"
	$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

$(BUILD):
	mkdir -p $@

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: ../stm/%.s
	$(AS) -o $@ $<

$(BUILD)/%.o: ../stm/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(PYSRC)/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(PYSRC)/%.c mpconfigport.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(CORE_PATH)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/emitnthumb.o: $(PYSRC)/emitnative.c $(PYSRC)/emit.h
	$(CC) $(CFLAGS) -DN_THUMB -c -o $@ $<

# optimising gc for speed; 5ms down to 4ms
$(BUILD)/gc.o: $(PYSRC)/gc.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

# optimising vm for speed, adds only a small amount to code size but makes a huge difference to speed (20% faster)
$(BUILD)/vm.o: $(PYSRC)/vm.c
	$(CC) $(CFLAGS) -O3 -c -o $@ $<

$(BUILD)/main.o: mpconfigport.h
$(BUILD)/parse.o: $(PYSRC)/grammar.h
$(BUILD)/compile.o: $(PYSRC)/grammar.h
$(BUILD)/emitbc.o: $(PYSRC)/emit.h

clean:
	/bin/rm -rf $(BUILD)
	/bin/rm -f $(PROG).elf $(PROG).hex $(PROG).map

.PHONY: clean
