include ../py/mkenv.mk

# Generate python script binary data (DEPRECATED)
#build/ProgramScript.c: py/Main.py
#	@echo "Generate program script"
#	@python py2c.py $< $@

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk
LD=$(CROSS_COMPILE)gcc

CROSS_COMPILE = arm-none-eabi-

INC =  -I.
INC += -I..
INC += -I../mphal
INC += -I../teensy
INC += -I../lib/mp-readline
INC += -Iboard_ciaa_edu_4337/inc/ -Ilpc_chip_43xx/inc/
INC += -I$(BUILD)

COPT += -DCORE_M4 -D__USE_LPCOPEN -D__USE_NEWLIB

ARCH_FLAGS = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
CFLAGS_CORTEX_M4 = $(ARCH_FLAGS) -fsingle-precision-constant -Wdouble-promotion -fshort-enums
CFLAGS = $(INC) -Wall -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
endif

LDFLAGS = $(ARCH_FLAGS) -nostdlib -T ciaa_lpc4337.ld -Wl,-Map=$@.map -Wl,--cref
LIBS =

SRC_C = \
	main.c \
	sysinit.c \
	system.c \
	cr_startup_lpc43xx.c \
	modpyb.c \
	ciaanxp_mphal.c \
	modpybled.c \
	stmhal/pyexec.c \
	stmhal/pybstdio.c \
	lib/mp-readline/readline.c \
	moduos.c \
	modpybswitch.c \
	modpybuart.c \
	modpybpin.c \
	modpybextint.c \
	modpybdac.c \
	modpybtimer.c \
	modpybpwm.c \
	$(wildcard lpc_chip_43xx/src/*.c) \
	$(wildcard board_ciaa_edu_4337/src/*.c) \
	teensy/memzip.c \
	memzip_extras.c \
	teensy/lexermemzip.c \
#	build/ProgramScript.c \
	printf.c \
	string0.c \
	malloc0.c \
	gccollect.c \

SRC_S = \
#	startup_stm32f40xx.s \
	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

all: $(BUILD)/firmware.elf

build/fs.zip: $(wildcard py/*)
	$(ECHO) "Create ROMFS (phase 1)"
	$(Q)(cd py && zip -0 -r -D ../$@ .)

build/fs.o: build/fs.zip
	$(ECHO) "Create ROMFS (phase 2)"
	$(Q)$(CROSS_COMPILE)objcopy -v -B arm -I binary -O elf32-littlearm $^ $@


$(BUILD)/firmware.elf: $(OBJ) build/fs.o
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) -A -x $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(CROSS_COMPILE)objcopy -v -O binary $< $@

PHONY: debug openocd download erase

download: $(BUILD)/firmware.bin
	@openocd -f ciaa-nxp.cfg \
		-c "init" \
		-c "halt 0" \
		-c "flash write_image erase unlock $^ 0x1A000000 bin" \
		-c "reset run" \
		-c "shutdown"

erase:
	@openocd -f ciaa-nxp.cfg \
		-c "init" -c "halt 0" -c "flash erase_sector 0 0 last" -c "shutdown"

openocd:
	@openocd -f ciaa-nxp.cfg

debug: $(BUILD)/firmware.elf
	$(CROSS_COMPILE)gdb $(BUILD)/firmware.elf -ex "target remote :3333" -ex "mon reset halt" -ex "load"

include ../py/mkrules.mk
