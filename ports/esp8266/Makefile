# Select the board to build for:
ifdef BOARD_DIR
# Custom board path - remove trailing slash and get the final component of
# the path as the board name.
BOARD ?= $(notdir $(BOARD_DIR:/=))
else
# If not given on the command line, then default to ESP8266_GENERIC.
BOARD ?= ESP8266_GENERIC
BOARD_DIR ?= boards/$(BOARD)
endif

ifeq ($(wildcard $(BOARD_DIR)/.),)
ifeq ($(findstring boards/GENERIC,$(BOARD_DIR)),boards/GENERIC)
$(warning The GENERIC* boards have been renamed to ESP8266_GENERIC)
endif
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

ifneq ($(BOARD_VARIANT),)
ifeq ($(wildcard $(BOARD_DIR)/mpconfigvariant_$(BOARD_VARIANT).mk),)
$(error Invalid BOARD_VARIANT specified: $(BOARD_VARIANT))
endif
endif

# If the build directory is not given, make it reflect the board name (and
# optionally the board variant).
ifneq ($(BOARD_VARIANT),)
BUILD ?= build-$(BOARD)-$(BOARD_VARIANT)
else
BUILD ?= build-$(BOARD)
endif

include ../../py/mkenv.mk

# Include board specific .mk file, and optional board variant .mk file.
include $(BOARD_DIR)/mpconfigboard.mk
ifeq ($(BOARD_VARIANT),)
-include $(BOARD_DIR)/mpconfigvariant.mk
else
include $(BOARD_DIR)/mpconfigvariant_$(BOARD_VARIANT).mk
endif

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h #$(BUILD)/pins_qstr.h
QSTR_GLOBAL_DEPENDENCIES = $(BOARD_DIR)/mpconfigboard.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1
MICROPY_PY_SSL = 1
MICROPY_SSL_AXTLS = 1
AXTLS_DEFS_EXTRA = -Dabort=abort_ -DRT_MAX_PLAIN_LENGTH=1024 -DRT_EXTRA=4096
BTREE_DEFS_EXTRA = -DMICROPY_BERKELEY_DB_DEFPSIZE=1024 -DMICROPY_BERKELEY_DB_MINCACHE=3

FROZEN_MANIFEST ?= boards/manifest.py

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

GIT_SUBMODULES += lib/axtls lib/berkeley-db-1.xx

FWBIN = $(BUILD)/firmware.bin
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

CFLAGS += $(INC) -Wall -Wpointer-arith -Werror -std=gnu99 -nostdlib -DUART_OS=$(UART_OS) \
	$(CFLAGS_XTENSA) $(COPT) $(CFLAGS_EXTRA) -I$(BOARD_DIR)

LD_FILES ?= boards/esp8266_2m.ld
LDFLAGS += -nostdlib -T $(LD_FILES) -Map=$(@:.elf=.map) --cref
LIBS += -L$(ESP_SDK)/lib -lmain -ljson -llwip_open -lpp -lnet80211 -lwpa -lphy -lnet80211

ifeq ($(MICROPY_PY_ESPNOW),1)
CFLAGS += -DMICROPY_PY_ESPNOW=1
LIBS += -lespnow
endif

LIBGCC_FILE_NAME = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
LIBS += -L$(dir $(LIBGCC_FILE_NAME)) -lgcc

# Debugging/Optimization
CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
COPT = -O0
else
CFLAGS += -fdata-sections -ffunction-sections
COPT += -Os -DNDEBUG
LDFLAGS += --gc-sections
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -Wmissing-prototypes -Wold-style-definition -std=gnu99,$(CFLAGS))

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
	machine_bitstream.c \
	machine_pin.c \
	machine_rtc.c \
	machine_spi.c \
	modesp.c \
	network_wlan.c \
	ets_alt_task.c \
	fatfs_port.c \
	posix_helpers.c \
	hspi.c \
	$(wildcard $(BOARD_DIR)/*.c) \

ifeq ($(MICROPY_PY_ESPNOW),1)
SRC_C += \
	modespnow.c
endif

LIB_SRC_C += $(SRC_LIB_LIBM_C)
LIB_SRC_C += $(SRC_LIB_LIBM_SQRT_SW_C)

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

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SHARED_SRC_C)

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

.PHONY: deploy

deploy: $(FWBIN)
	$(ECHO) "Writing $< to the board"
	$(Q)esptool.py --port $(PORT) --baud $(BAUD) write_flash --verify --flash_size=$(FLASH_SIZE) --flash_mode=$(FLASH_MODE) 0 $<

erase:
	$(ECHO) "Erase flash"
	$(Q)esptool.py --port $(PORT) --baud $(BAUD) erase_flash

reset:
	echo -e "\r\nimport machine; machine.reset()\r\n" >$(PORT)

ifeq ($(BOARD_VARIANT),OTA)
$(FWBIN): $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)esptool.py elf2image $^
	$(Q)$(PYTHON) makeimg.py $(BUILD)/firmware.elf-0x00000.bin $(BUILD)/firmware.elf-0x[0-5][1-f]000.bin $(BUILD)/firmware-ota.bin

	$(Q)cat $(YAOTA8266)/yaota8266.bin $(BUILD)/firmware-ota.bin > $@
	$(Q)$(PYTHON) $(YAOTA8266)/ota-client/ota_client.py sign $@
else
$(FWBIN): $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)esptool.py elf2image $^
	$(Q)$(PYTHON) makeimg.py $(BUILD)/firmware.elf-0x00000.bin $(BUILD)/firmware.elf-0x[0-5][1-f]000.bin $@
endif

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

include $(TOP)/py/mkrules.mk

clean-modules:
	git clean -f -d modules
	rm -f $(BUILD)/frozen*.c
