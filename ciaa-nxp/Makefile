include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk
LD=$(CROSS_COMPILE)gcc

CROSS_COMPILE = arm-none-eabi-

INC =  -I.
INC += -I..
INC += -I../stmhal
INC += -I../teensy
INC += -I../lib/mp-readline
INC += -I../lib/timeutils
INC += -Iboard_ciaa_edu_4337/inc/ -Ilpc_chip_43xx/inc/
INC += -I$(BUILD)

COPT += -DCORE_M4 -D__USE_LPCOPEN -D__USE_NEWLIB

ARCH_FLAGS = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard 
CFLAGS_CORTEX_M4 = $(ARCH_FLAGS) -fsingle-precision-constant -Wdouble-promotion -fshort-enums
CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -ansi -std=gnu99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)

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
	ciaanxp_mphal.c \
	ciaa_help.c \
	bootSys.c  \
	lpc43_diskio.c \
	lpc43_flash.c \
	lpc43_storage.c \
	ffconf.c \
	moduos.c \
	modutime.c \
	modpyb.c \
	modpybled.c \
	modpybswitch.c \
	modpybuart.c \
	modpybpin.c \
	modpybextint.c \
	modpybdac.c \
	modpybtimer.c \
	modpybpwm.c \
	modpybadc.c \
	modpybkeyboard.c \
	modpyblcd44780u.c \
	modpybeeprom.c \
	modpybspi.c \
	modpybrtc.c \
	modpybi2c.c \
	lib/fatfs/ff.c \
	lib/fatfs/option/ccsbcs.c \
	lib/mp-readline/readline.c \
	lib/timeutils/timeutils.c \
	lib/utils/pyexec.c \
	stmhal/file.c \
	stmhal/import.c \
	stmhal/input.c \
	stmhal/lexerfatfs.c \
	stmhal/pybstdio.c \
	stmhal/bufhelper.c \
	$(wildcard lpc_chip_43xx/src/*.c) \
	$(wildcard board_ciaa_edu_4337/src/*.c) \
	$(wildcard board_ciaa_edu_4337/src/ponchos/*.c)

SRC_S = \
#	startup_stm32f40xx.s \
	gchelper.s \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o) $(SRC_S:.s=.o))

all: $(BUILD)/firmware.elf

# Frozen ####################################################################

OBJ += $(BUILD)/frozen-files.o
MAKE_FROZEN = tools/make-frozen.py

ifeq ($(FROZEN_DIR),)
FROZEN_DIR = frozen
endif

$(BUILD)/frozen-files.o: $(BUILD)/frozen-files.c
	$(call compile_c)

$(BUILD)/frozen-files.c: $(shell find ${FROZEN_DIR} -type f)
	@$(ECHO) "Creating $@"
	$(Q)$(PYTHON) $(MAKE_FROZEN) $(FROZEN_DIR) > $@

# Frozen ####################################################################

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(Q)$(SIZE) -A -x $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(CROSS_COMPILE)objcopy -v -O binary $< $@

PHONY: debug openocd download erase erase_fs

download: $(BUILD)/firmware.bin
	@openocd -f ciaa-nxp.cfg \
		-c "init" \
		-c "halt 0" \
		-c "flash write_image erase unlock $^ 0x1A000000 bin" \
		-c "reset run" \
		-c "shutdown"

fsdownload: $(BUILD)/fs.bin
	@openocd -f ciaa-nxp.cfg \
		-c "init" \
		-c "halt 0" \
		-c "flash write_image erase unlock $^ 0x1B000000 bin" \
		-c "reset run" \
		-c "shutdown"

erase:
	@openocd -f ciaa-nxp.cfg \
		-c "init" -c "halt 0" -c "flash erase_sector 0 0 last" -c "shutdown"

erase_fs:
	@openocd -f ciaa-nxp.cfg \
		-c "init" -c "halt 0" -c "flash erase_sector 1 0 last" -c "shutdown"

openocd:
	@openocd -f ciaa-nxp.cfg

debug: $(BUILD)/firmware.elf
	$(CROSS_COMPILE)gdb $(BUILD)/firmware.elf -ex "target remote :3333" -ex "mon reset halt" -ex "load"

include ../py/mkrules.mk
