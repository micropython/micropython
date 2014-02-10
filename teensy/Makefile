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
CFLAGS = -I. -I$(PY_SRC) -I$(CORE_PATH) -Wall -ansi -std=gnu99 $(CFLAGS_CORTEX_M4)
LDFLAGS = -nostdlib -T mk20dx256.ld
LIBS = -L $(COMPILER_PATH)/../lib/gcc/arm-none-eabi/4.7.2/thumb2 -lgcc

#Debugging/Optimization
ifdef DEBUG
CFLAGS += -Og -ggdb
else
CFLAGS += -Os #-DNDEBUG
endif

SRC_C = \
	main.c \
	lcd.c \
	led.c \
	lexerfatfs.c \
	lexermemzip.c \
	memzip.c \
	servo.c \
	usart.c \
	usb.c \

STM_SRC_C = $(addprefix stm/,\
	malloc0.c \
	printf.c \
	string0.c \
	)

STM_SRC_S = $(addprefix stm/,\
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
#LIB = -lreadline
# the following is needed for BSD
#LIB += -ltermcap

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

$(BUILD)/%.o: $(CORE_PATH)/%.c
	$(call compile_c)

include ../py/mkrules.mk
