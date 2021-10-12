# Select the board to build for: if not given on the command line,
# then default to GENERIC.
BOARD ?= GENERIC

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

BOARD_DIR ?= boards/$(BOARD)
ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ../../py/mkenv.mk

# Optional
-include $(BOARD_DIR)/mpconfigboard.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h #$(BUILD)/pins_qstr.h
QSTR_GLOBAL_DEPENDENCIES = $(BOARD_DIR)/mpconfigboard.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1
MICROPY_PY_USSL = 1
MICROPY_SSL_AXTLS = 1
AXTLS_DEFS_EXTRA = -Dabort=abort_ -DRT_MAX_PLAIN_LENGTH=1024 -DRT_EXTRA=4096
BTREE_DEFS_EXTRA = -DDEFPSIZE=1024 -DMINCACHE=3

FROZEN_MANIFEST ?= boards/manifest.py
FROZEN_DIR ?=
FROZEN_MPY_DIR ?=

# include py core make definitions
include $(TOP)/py/py.mk

GIT_SUBMODULES = lib/axtls lib/berkeley-db-1.xx

FWBIN = $(BUILD)/firmware-combined.bin
PORT ?= /dev/ttyACM0
BAUD ?= 115200
FLASH_MODE ?= qio
FLASH_SIZE ?= detect
CROSS_COMPILE ?= xtensa-lx106-elf-
ESP_SDK = $(shell $(CC) -print-sysroot)/usr

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(ESP_SDK)/include

# UART for "os" messages. 0 is normal UART as used by MicroPython REPL,
# 1 is debug UART (tx only), -1 to disable.
UART_OS = -1

CFLAGS_XTENSA = -fsingle-precision-constant -Wdouble-promotion \
	-D__ets__ -DICACHE_FLASH \
	-fno-inline-functions \
	-Wl,-EL -mlongcalls -mtext-section-literals -mforce-l32 \
	-DLWIP_OPEN_SRC

CFLAGS = $(INC) -Wall -Wpointer-arith -Werror -std=gnu99 -nostdlib -DUART_OS=$(UART_OS) \
	$(CFLAGS_XTENSA) $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA) -I$(BOARD_DIR)

LD_FILES ?= boards/esp8266_2m.ld
LDFLAGS = -nostdlib -T $(LD_FILES) -Map=$(@:.elf=.map) --cref
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

# Options for mpy-cross
MPY_CROSS_FLAGS += -march=xtensa

SRC_C = \
	strtoll.c \
	main.c \
	help.c \
	esp_mphal.c \
	esp_init_data.c \
	gccollect.c \
	lexerstr32.c \
	uart.c \
	esppwm.c \
	espapa102.c \
	modpyb.c \
	modmachine.c \
	machine_bitstream.c \
	machine_pin.c \
	machine_rtc.c \
	machine_adc.c \
	machine_uart.c \
	machine_wdt.c \
	machine_hspi.c \
	modesp.c \
	modnetwork.c \
	modutime.c \
	moduos.c \
	ets_alt_task.c \
	fatfs_port.c \
	posix_helpers.c \
	hspi.c \
	$(wildcard $(BOARD_DIR)/*.c) \
	$(SRC_MOD)

EXTMOD_SRC_C = $(addprefix extmod/,\
	modlwip.c \
	modonewire.c \
        )

LIB_SRC_C = $(addprefix lib/,\
	libm/math.c \
	libm/fmodf.c \
	libm/nearbyintf.c \
	libm/ef_sqrt.c \
	libm/erf_lgamma.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/sf_erf.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/acoshf.c \
	libm/asinfacosf.c \
	libm/asinhf.c \
	libm/atanf.c \
	libm/atanhf.c \
	libm/atan2f.c \
	libm/log1pf.c \
	libm/roundf.c \
	libm/wf_lgamma.c \
	libm/wf_tgamma.c \
	)

SHARED_SRC_C = $(addprefix shared/,\
	libc/__errno.c \
	libc/string0.c \
	netutils/netutils.c \
	readline/readline.c \
	runtime/interrupt_char.c \
	runtime/pyexec.c \
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	timeutils/timeutils.c \
	)

DRIVERS_SRC_C = $(addprefix drivers/,\
	bus/softspi.c \
	dht/dht.c \
        )

SRC_S = \
	gchelper.s \

OBJ =
OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(EXTMOD_SRC_C) $(SHARED_SRC_C) $(DRIVERS_SRC_C)
# Append any auto-generated sources that are needed by sources listed in SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

all: $(FWBIN)

CONFVARS_FILE = $(BUILD)/confvars

ifeq ($(wildcard $(CONFVARS_FILE)),)
$(shell $(MKDIR) -p $(BUILD))
$(shell echo $(FROZEN_MANIFEST) $(UART_OS) > $(CONFVARS_FILE))
else ifneq ($(shell cat $(CONFVARS_FILE)), $(FROZEN_MANIFEST) $(UART_OS))
$(shell echo $(FROZEN_MANIFEST) $(UART_OS) > $(CONFVARS_FILE))
endif

$(BUILD)/uart.o: $(CONFVARS_FILE)

FROZEN_EXTRA_DEPS = $(CONFVARS_FILE)

ifneq ($(FROZEN_MANIFEST)$(FROZEN_MPY_DIR),)
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
endif

ifneq ($(FROZEN_MANIFEST)$(FROZEN_DIR),)
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
endif

.PHONY: deploy

deploy: $(BUILD)/firmware-combined.bin
	$(ECHO) "Writing $< to the board"
	$(Q)esptool.py --port $(PORT) --baud $(BAUD) write_flash --verify --flash_size=$(FLASH_SIZE) --flash_mode=$(FLASH_MODE) 0 $<

erase:
	$(ECHO) "Erase flash"
	$(Q)esptool.py --port $(PORT) --baud $(BAUD) erase_flash

reset:
	echo -e "\r\nimport machine; machine.reset()\r\n" >$(PORT)

$(FWBIN): $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)esptool.py elf2image $^
	$(Q)$(PYTHON) makeimg.py $(BUILD)/firmware.elf-0x00000.bin $(BUILD)/firmware.elf-0x[0-5][1-f]000.bin $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

ota:
	rm -f $(BUILD)/firmware.elf $(BUILD)/firmware.elf*.bin
	$(MAKE) LD_FILES=boards/esp8266_ota.ld FWBIN=$(BUILD)/firmware-ota.bin

include $(TOP)/py/mkrules.mk

clean-modules:
	git clean -f -d modules
	rm -f $(BUILD)/frozen*.c
