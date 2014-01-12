# define main target
all: all2

# include py core make definitions
include ../py/py.mk

# program for deletion
RM = /bin/rm
ECHO = @echo

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
	lexermemzip.c \
	memzip.c \
	servo.c \
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
hex: $(BUILD)/micropython-mz.hex

post_compile: $(BUILD)/micropython-mz.hex
	$(ECHO) "Preparing $@ for upload"
	$(Q)$(TOOLS_PATH)/teensy_post_compile -file="$(basename $(<F))" -path="$(<D)" -tools="$(TOOLS_PATH)"

reboot:
	$(ECHO) "REBOOT"
	-$(Q)$(TOOLS_PATH)/teensy_reboot

upload: post_compile reboot

$(BUILD)/micropython.elf: $(OBJ)
	$(ECHO) "LINK $<"
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

$(BUILD)/%.o: %.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: ../stm/%.s
	$(ECHO) "AS $<"
	$(Q)$(AS) -o $@ $<

$(BUILD)/%.o: ../stm/%.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(CORE_PATH)/%.c
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/main.o: mpconfigport.h

clean:
	$(RM) -rf $(BUILD)

.PHONY: all all2 clean
