include ../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h #$(BUILD)/pins_qstr.h

MICROPY_PY_USSL = 1

# include py core make definitions
include ../py/py.mk

MPY_CROSS = ../mpy-cross/mpy-cross
MPY_TOOL = ../tools/mpy-tool.py
MAKE_FROZEN = ../tools/make-frozen.py

SCRIPTDIR = scripts
FROZEN_MPY_DIR = modules
PORT ?= /dev/ttyACM0
BAUD ?= 115200
CROSS_COMPILE = xtensa-lx106-elf-
ESP_SDK = $(shell $(CC) -print-sysroot)/usr

INC += -I.
INC += -I..
INC += -I../stmhal
INC += -I../lib/mp-readline
INC += -I../lib/netutils
INC += -I../lib/timeutils
INC += -I$(BUILD)
INC += -I$(ESP_SDK)/include

# UART for "os" messages. 0 is normal UART as used by MicroPython REPL,
# 1 is debug UART (tx only), -1 to disable.
UART_OS = 0

CFLAGS_XTENSA = -fsingle-precision-constant -Wdouble-promotion \
	-D__ets__ -DICACHE_FLASH \
	-fno-inline-functions \
	-Wl,-EL -mlongcalls -mtext-section-literals -mforce-l32 \
	-DLWIP_OPEN_SRC

CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -ansi -std=gnu99 -nostdlib -DUART_OS=$(UART_OS) \
	$(CFLAGS_XTENSA) $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)

LDFLAGS = -nostdlib -T esp8266.ld -Map=$(@:.elf=.map) --cref
LIBS = -L$(ESP_SDK)/lib -lmain -ljson -llwip_open -lpp -lnet80211 -lwpa -lphy -lnet80211 $(LDFLAGS_MOD)

LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
LIBS += -L$(dir $(LIBGCC_FILE_NAME)) -lgcc

# Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -g
COPT = -O0
else
CFLAGS += -fdata-sections -ffunction-sections
COPT += -Os -DNDEBUG
LDFLAGS += --gc-sections
endif

SRC_C = \
	strtoll.c \
	main.c \
	help.c \
	esp_mphal.c \
	gccollect.c \
	lexerstr32.c \
	uart.c \
	esppwm.c \
	esponewire.c \
	espneopixel.c \
	espapa102.c \
	intr.c \
	modpyb.c \
	modpybpin.c \
	modpybpwm.c \
	modpybrtc.c \
	modpybadc.c \
	modpybuart.c \
	modpybspi.c \
	modesp.c \
	modnetwork.c \
	modutime.c \
	moduos.c \
	modmachine.c \
	modonewire.c \
	ets_alt_task.c \
	$(BUILD)/frozen.c \
	fatfs_port.c \
	axtls_helpers.c \

STM_SRC_C = $(addprefix stmhal/,\
	pybstdio.c \
	input.c \
	)

EXTMOD_SRC_C = $(addprefix extmod/,\
	modlwip.c \
        )

LIB_SRC_C = $(addprefix lib/,\
	libc/string0.c \
	libm/math.c \
	libm/fmodf.c \
	libm/roundf.c \
	libm/ef_sqrt.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
	mp-readline/readline.c \
	netutils/netutils.c \
	timeutils/timeutils.c \
	utils/pyexec.c \
	utils/pyhelp.c \
	utils/printf.c \
	fatfs/ff.c \
	fatfs/option/ccsbcs.c \
	)

DRIVERS_SRC_C = $(addprefix drivers/,\
	dht/dht.c \
        )

SRC_S = \
	gchelper.s \

FROZEN_MPY_PY_FILES := $(shell find $(FROZEN_MPY_DIR)/ -type f -name '*.py')
FROZEN_MPY_MPY_FILES := $(addprefix $(BUILD)/,$(FROZEN_MPY_PY_FILES:.py=.mpy))

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(STM_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))
OBJ += $(BUILD)/$(BUILD)/frozen_mpy.o
#OBJ += $(BUILD)/pins_$(BOARD).o

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(STM_SRC_C) $(EXTMOD_SRC_C) $(DRIVERS_SRC_C)
# Append any auto-generated sources that are needed by sources listed in SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

all: $(BUILD)/firmware-combined.bin

CONFVARS_FILE = $(BUILD)/confvars

ifeq ($(wildcard $(CONFVARS_FILE)),)
$(shell $(MKDIR) -p $(BUILD))
$(shell echo $(SCRIPTDIR) $(UART_OS) > $(CONFVARS_FILE))
else ifneq ($(shell cat $(CONFVARS_FILE)), $(SCRIPTDIR) $(UART_OS))
$(shell echo $(SCRIPTDIR) $(UART_OS) > $(CONFVARS_FILE))
endif

$(BUILD)/uart.o: $(CONFVARS_FILE)

$(BUILD)/frozen.c: $(wildcard $(SCRIPTDIR)/*) $(CONFVARS_FILE)
	$(ECHO) "Generating $@"
	$(Q)$(MAKE_FROZEN) $(SCRIPTDIR) > $@

# to build .mpy files from .py files
$(BUILD)/$(FROZEN_MPY_DIR)/%.mpy: $(FROZEN_MPY_DIR)/%.py
	@$(ECHO) "MPY $<"
	$(Q)$(MKDIR) -p $(dir $@)
	$(Q)$(MPY_CROSS) -o $@ -s $(^:$(FROZEN_MPY_DIR)/%=%) $^

# to build frozen_mpy.c from all .mpy files
$(BUILD)/frozen_mpy.c: $(FROZEN_MPY_MPY_FILES) $(BUILD)/genhdr/qstrdefs.generated.h
	@$(ECHO) "Creating $@"
	$(Q)$(PYTHON) $(MPY_TOOL) -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h $(FROZEN_MPY_MPY_FILES) > $@

.PHONY: deploy

deploy: $(BUILD)/firmware-combined.bin
	$(ECHO) "Writing $< to the board"
	$(Q)esptool.py --port $(PORT) --baud $(BAUD) write_flash --verify --flash_size=8m 0 $<
	#$(Q)esptool.py --port $(PORT) --baud $(BAUD) write_flash --flash_size=8m 0 $(BUILD)/firmware.elf-0x00000.bin 0x9000 $(BUILD)/firmware.elf-0x0[1-f]000.bin

reset:
	echo -e "\r\nimport machine; machine.reset()\r\n" >$(PORT)

$(BUILD)/firmware-combined.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)esptool.py elf2image $^
	$(Q)$(PYTHON) makeimg.py $(BUILD)/firmware.elf-0x00000.bin $(BUILD)/firmware.elf-0x0[1-f]000.bin $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

#MAKE_PINS = boards/make-pins.py
#BOARD_PINS = boards/$(BOARD)/pins.csv
#AF_FILE = boards/stm32f4xx_af.csv
#PREFIX_FILE = boards/stm32f4xx_prefix.c
#GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
#GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
#GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
#GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
#GEN_PINS_AF_PY = $(BUILD)/pins_af.py

# Making OBJ use an order-only depenedency on the generated pins.h file
# has the side effect of making the pins.h file before we actually compile
# any of the objects. The normal dependency generation will deal with the
# case when pins.h is modified. But when it doesn't exist, we don't know
# which source files might need it.
#$(OBJ): | $(HEADER_BUILD)/pins.h

# Use a pattern rule here so that make will only call make-pins.py once to make
# both pins_$(BOARD).c and pins.h
#$(BUILD)/%_$(BOARD).c $(HEADER_BUILD)/%.h $(HEADER_BUILD)/%_af_const.h $(BUILD)/%_qstr.h: boards/$(BOARD)/%.csv $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
#	$(ECHO) "Create $@"
#	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) --prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) --af-const $(GEN_PINS_AF_CONST) --af-py $(GEN_PINS_AF_PY) > $(GEN_PINS_SRC)
#
#$(BUILD)/pins_$(BOARD).o: $(BUILD)/pins_$(BOARD).c
#	$(call compile_c)

include ../py/mkrules.mk

axtls:
	cd ../lib/axtls; cp config/upyconfig config/.config
	cd ../lib/axtls; make oldconfig -B
	cd ../lib/axtls; make clean
	cd ../lib/axtls; make all CC="$(CC)" LD="$(LD)" AR="$(AR)" CFLAGS_EXTRA="$(CFLAGS_XTENSA) -Dabort=abort_ -DRT_MAX_PLAIN_LENGTH=1024 -DRT_EXTRA=3072"
