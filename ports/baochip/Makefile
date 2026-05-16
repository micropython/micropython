# Makefile for the baochip MicroPython port (Baochip-1x RISC-V SoC).

BOARD ?= DABAO
BOARD_DIR ?= boards/$(BOARD)

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

BUILD ?= build-$(BOARD)

include ../../py/mkenv.mk
include mpconfigport.mk
include $(BOARD_DIR)/mpconfigboard.mk

# Vendor SDK location, both as a TOP-relative path (used to build OBJ
# paths that land under $(BUILD)/lib/dabao-sdk/...) and as an absolute
# path (used for the -I header search list).
DABAO_SDK = lib/dabao-sdk
DABAO_SDK_DIR = $(TOP)/$(DABAO_SDK)

GIT_SUBMODULES += $(DABAO_SDK)

# qstr definitions (must come before including py.mk).
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations.
MICROPY_FLOAT_IMPL ?= none

FROZEN_MANIFEST ?= boards/manifest.py

include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

# Default to the Debian-packaged toolchain (matches upstream CI) but
# auto-detect xpack's prefix if that's the only one on PATH.
ifeq ($(origin CROSS_COMPILE), undefined)
ifneq ($(shell which riscv64-unknown-elf-gcc 2>/dev/null),)
CROSS_COMPILE := riscv64-unknown-elf-
else ifneq ($(shell which riscv-none-elf-gcc 2>/dev/null),)
CROSS_COMPILE := riscv-none-elf-
else
$(error No RISC-V GCC found on PATH (looked for riscv64-unknown-elf- and riscv-none-elf-))
endif
endif

# RV32IMAC + Zicsr/Zifencei, ILP32 ABI, soft-float -- matches what
# the Dabao SDK ships and what boot0/boot1 expects.
RV32_ARCH = rv32imac_zicsr_zifencei
RV32_ABI  = ilp32

AFLAGS  += -mabi=$(RV32_ABI) -march=$(RV32_ARCH)
CFLAGS  += $(AFLAGS) -ffreestanding -nostdlib -Os -Wall -Wextra -DNDEBUG
LDFLAGS += -mabi=$(RV32_ABI) -march=$(RV32_ARCH) -nostdlib -Wl,-EL

# Pick up picolibc's headers when the toolchain has them but they aren't
# on the default include path (Debian's gcc-riscv64-unknown-elf needs
# picolibc-riscv64-unknown-elf + the specs flag; xpack riscv-none-elf
# bundles newlib and finds <stdio.h> etc unaided).
PICOLIBC_SPECS = $(shell $(CC) --print-file-name=picolibc.specs)
ifneq ($(PICOLIBC_SPECS),picolibc.specs)
SPECS_FRAGMENT = --specs=$(PICOLIBC_SPECS)
CFLAGS  += $(SPECS_FRAGMENT)
LDFLAGS += $(SPECS_FRAGMENT)
endif

# libgcc provides compiler builtins like __clzsi2, __udivdi3 that GCC
# can emit calls to even with -ffreestanding.
LIBS = $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)

# Vendor SDK header search paths. We selectively compile SDK .c files
# (see SDK_SRC below) -- we do not link a prebuilt libdabao_sdk.
INC += -I.
INC += -I$(BUILD)
INC += -I$(TOP)
INC += -I$(BOARD_DIR)
INC += -I$(DABAO_SDK_DIR)/src/sevs
INC += -I$(DABAO_SDK_DIR)/src/common/bao_base/include
INC += -I$(DABAO_SDK_DIR)/src/common/bao_stdlib/include
INC += -I$(DABAO_SDK_DIR)/src/bao1x/hardware_regs/include
INC += -I$(DABAO_SDK_DIR)/src/bao1x/hardware_irq/include
INC += -I$(DABAO_SDK_DIR)/src/bao1x/hardware_gpio/include
INC += -I$(DABAO_SDK_DIR)/src/bao1x/hardware_uart/include
INC += -I$(DABAO_SDK_DIR)/src/bao1x/hardware_timer/include
INC += -I$(DABAO_SDK_DIR)/src/bao1x/hardware_reset/include
INC += -I$(DABAO_SDK_DIR)/src/boards/include

CFLAGS += $(INC)

# Port C sources.
SRC_C = \
    main.c \
    machine_pin.c \
    mphalport.c \

# SDK sources compiled directly into the firmware. Listed here per file
# so the build picks up only what we actually use.
# NOTE: never compile $(DABAO_SDK_DIR)/src/common/bao_stdlib/stdlib.c --
# it defines a weak trap_dispatch that conflicts with the real one in
# hardware_irq/irq.c, plus SDK-style wrappers (bao_init, mini_printf)
# that we replace with MicroPython idioms.
SDK_SRC = $(addprefix $(DABAO_SDK)/, \
    src/bao1x/hardware_irq/irq.c \
    src/bao1x/hardware_uart/uart.c \
    src/bao1x/hardware_gpio/gpio.c \
    src/sevs/sevs_assert_target.c \
    )

# Assembly sources.
SRC_S = $(addprefix $(DABAO_SDK)/, \
    src/runtime/crt0.S \
    )

SRC_S += shared/runtime/gchelper_rv32i.s

SRC_C += \
    shared/libc/string0.c \
    shared/runtime/gchelper_native.c \
    shared/runtime/interrupt_char.c \
    shared/runtime/pyexec.c \
    shared/runtime/stdout_helpers.c \
    shared/runtime/sys_stdio_mphal.c \
    shared/readline/readline.c \

# Pin generation
GEN_PIN_MKPINS     = boards/make-pins.py
GEN_PIN_PREFIX     = boards/baochip_prefix.c
GEN_PINS_BOARD_CSV = $(BOARD_DIR)/pins.csv
GEN_PINS_SRC       = $(BUILD)/pins.c
GEN_PINS_HDR       = $(HEADER_BUILD)/pins.h

SRC_QSTR += $(SRC_C) $(GEN_PINS_SRC)

OBJ  = $(PY_O)
OBJ += $(GEN_PINS_SRC:.c=.o)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SDK_SRC:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(patsubst %.S,%.o, $(patsubst %.s,%.o, $(SRC_S))))

LDFLAGS += -T baochip.ld

.PHONY: all
all: $(BUILD)/firmware.uf2

$(BUILD)/firmware.elf: $(OBJ) baochip.ld
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS) -Wl,-Map=$(@:.elf=.map)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $< $@

# Sign + UF2-wrap the bare binary.  NOTE: the signing key is the public
# SDK developer Ed25519ph key (committed in lib/dabao-sdk/keys/) so the
# resulting signature provides NO meaningful attestation -- it just makes
# the UF2 acceptable to boot1.  Any operational use must swap in a project
# key.  The 0x60060000 base and 0xa7d76373 family-id are SDK constants.
$(BUILD)/firmware.uf2: $(BUILD)/firmware.bin
	$(ECHO) "SIGN $@"
	$(Q)$(PYTHON) $(DABAO_SDK_DIR)/tools/sign_and_uf2.py $< $@ 0x60060000 0xa7d76373

# Flash via the chip's native USB-CDC.  Expects the chip to already be in
# boot1: call machine.bootloader() from a running REPL, or hold PROG, tap
# RESET, and release PROG.  PERSIST=1 (the default) leaves bootwait
# disabled so the chip auto-boots user firmware after flashing.
DABAO_PORT ?=
PERSIST ?= 1

.PHONY: deploy
deploy: $(BUILD)/firmware.uf2
	$(if $(DABAO_PORT),,$(error Set DABAO_PORT to the boot1 CDC serial port))
	$(Q)$(PYTHON) $(DABAO_SDK_DIR)/tools/serial_flash.py \
	    $(if $(filter 1,$(PERSIST)),--persistent) \
	    $(DABAO_PORT) $<

# Pattern rule: boardgen generates both pins.c and genhdr/pins.h in one pass.
$(BUILD)/pins.c $(HEADER_BUILD)/pins.h: $(GEN_PINS_BOARD_CSV) $(GEN_PIN_MKPINS) $(GEN_PIN_PREFIX) | $(HEADER_BUILD)
	$(ECHO) "GEN $@"
	$(Q)$(PYTHON) $(GEN_PIN_MKPINS) \
	    --board-csv $(GEN_PINS_BOARD_CSV) \
	    --prefix $(GEN_PIN_PREFIX) \
	    --output-source $(GEN_PINS_SRC) \
	    --output-header $(GEN_PINS_HDR)

include $(TOP)/py/mkrules.mk
