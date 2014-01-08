# define main target
all: all2

# include py core make definitions
include ../py/py.mk

# program for deletion
RM = /bin/rm

ifeq ($(ARDUINO),)
$(error Please define ARDUINO (where TeensyDuino is installed))
endif
TOOLS_PATH = $(ARDUINO)/hardware/tools
COMPILER_PATH = $(TOOLS_PATH)/arm-none-eabi/bin
CORE_PATH = $(ARDUINO)/hardware/teensy/cores/teensy3
TARGET=TEENSY

AS = $(COMPILER_PATH)/arm-none-eabi-as
CC = $(COMPILER_PATH)/arm-none-eabi-gcc
LD = $(COMPILER_PATH)/arm-none-eabi-ld
OBJCOPY = $(COMPILER_PATH)/arm-none-eabi-objcopy
SIZE = $(COMPILER_PATH)/arm-none-eabi-size

CFLAGS_TEENSY = -DF_CPU=96000000 -DUSB_SERIAL -D__MK20DX256__
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -fsingle-precision-constant -Wdouble-promotion $(CFLAGS_TEENSY)
CFLAGS = -I. -I$(PY_SRC) -I$(CORE_PATH) -Wall -ansi -std=gnu99 -Os -DNDEBUG $(CFLAGS_CORTEX_M4) -D$(TARGET)
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

SRC_TEENSY = \
	mk20dx128.c \
	pins_teensy.c \
	analog.c \
	usb_desc.c \
	usb_dev.c \
	usb_mem.c \
	usb_serial.c \
	yield.c \

OBJ = $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(STM_SRC_C:.c=.o) $(STM_SRC_S:.s=.o) $(SRC_TEENSY:.c=.o)) $(PY_O)
#LIB = -lreadline
# the following is needed for BSD
#LIB += -ltermcap

all2: $(BUILD) hex
hex: $(BUILD)/flash.hex

post_compile: $(BUILD)/flash.hex
	$(TOOLS_PATH)/teensy_post_compile -file="$(basename $<)" -path="$(BUILD)" -tools="$(TOOLS_PATH)"

reboot:
	-$(TOOLS_PATH)/teensy_reboot

upload: post_compile reboot

$(BUILD)/flash.elf: $(OBJ)
	$(CC) $(LDFLAGS) -o "$@" -Wl,-Map,$(@:.elf=.map) $(OBJ) $(LIBS)
	$(SIZE) $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

$(BUILD)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: ../stm/%.s
	$(AS) -o $@ $<

$(BUILD)/%.o: ../stm/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(CORE_PATH)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/main.o: mpconfigport.h

clean:
	/bin/rm -rf $(BUILD)

.PHONY: all all2 clean
