# Include the core environment definitions; this will set $(TOP).
include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# Include py core make definitions.
include $(TOP)/py/py.mk

# Path you your toolchain and openocd installation, leave empty if already in system PATH
TOOLCHAIN_ROOT = /opt/wch/mounriver-studio-toolchain-riscv/bin/
OPENOCD_ROOT   = /opt/wch/mounriver-studio-toolchain-openocd/bin/

CROSS_COMPILE ?= $(TOOLCHAIN_ROOT)riscv-none-embed-

###############################################################################

# Project specific
BOARD ?= CH32V307EVT
BUILD ?= build-$(BOARD)

TARGET = $(BOARD).elf

SRC_DIR = bsp/src/
INC_DIR = bsp/inc/

# Toolchain
CC = $(TOOLCHAIN_ROOT)riscv-none-embed-gcc
DB = $(TOOLCHAIN_ROOT)riscv-none-embed-gdb

# Project sources
SRC_C = \
	main.c \
	mphalport.c \
	pin.c \
	pin_defs_ch32.c \
	pin_named_pins.c \
	modmachine.c \
	modutime.c \
	extmod/utime_mphal.c \
	shared/libc/string0.c \
	shared/readline/readline.c \
	shared/runtime/gchelper_generic.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
    
SRC_C += $(wildcard $(SRC_DIR)*.c) 

SRC_S += bsp/startup_ch32v30x_D8C.S

SRC_C += $(SRC_MOD)
SRC_CXX += $(SRC_MOD_CXX)
SRC_QSTR += $(SRC_MOD) $(SRC_MOD_CXX)
SRC_QSTR += pin.c pin_named_pins.c modmachine.c modutime.c

SRC_C += $(BUILD)/pins_$(BOARD).c
SRC_QSTR += $(BUILD)/pins_$(BOARD).c

# Project includes
INCLUDES = -I. -I$(INC_DIR) -I$(BUILD) -I$(TOP)

# Linker Script
LD_SCRIPT = bsp/ch32v30x.ld

# Compiler Flags
CFLAGS  = -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os
CFLAGS += -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -g 
CFLAGS += -std=gnu99 
CFLAGS += $(INCLUDES)

# Linker Flags
LDFLAGS = -T $(LD_SCRIPT) -nostartfiles -Xlinker --gc-sections --specs=nano.specs --specs=nosys.specs

OBJ = $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.S=.o))

###############################################################################

MAKE_PINS = boards/make-pins.py
BOARD_PINS = boards/ch32v307_evt_pins.csv
AF_FILE = boards/ch32v30x_af.csv
PREFIX_FILE = boards/prefix.c

GEN_PINS_SRC = $(BUILD)/pins_$(BOARD).c
GEN_PINS_HDR = $(HEADER_BUILD)/pins.h
GEN_PINS_QSTR = $(BUILD)/pins_qstr.h
GEN_PINS_AF_CONST = $(HEADER_BUILD)/pins_af_const.h
GEN_PINS_AF_DEFS = $(HEADER_BUILD)/pins_af_defs.h
GEN_PINS_AF_PY = $(BUILD)/pins_af.py

# Generate Pin details before QSTR processing
$(SRC_QSTR): | $(GEN_PINS_HDR) 

$(GEN_PINS_HDR): $(BOARD_PINS) $(MAKE_PINS) $(AF_FILE) $(PREFIX_FILE) | $(HEADER_BUILD)
	$(ECHO) "[make-pin] - GEN $@"
	$(Q)$(PYTHON) $(MAKE_PINS) --board $(BOARD_PINS) --af $(AF_FILE) \
		--prefix $(PREFIX_FILE) --hdr $(GEN_PINS_HDR) --hdr-obj-decls \
		--qstr $(GEN_PINS_QSTR) --af-const $(GEN_PINS_AF_CONST) \
		--af-defs $(GEN_PINS_AF_DEFS) --af-defs-cmp-strings \
		--af-py $(GEN_PINS_AF_PY) > $(GEN_PINS_SRC)

###############################################################################

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJ) 
	$(ECHO) "LINK $@"	
	$(Q)$(CC) $(LDFLAGS) -o $@ $^
	$(Q)$(SIZE) $@

prog: $(BUILD)/$(TARGET)
	$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c halt -c "flash erase_sector wch_riscv 0 last " -c exit
	
	#$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c halt -c "program $(BUILD)/$(TARGET) 0x08000000" -c exit # OpenOCD-wch 0.10 (MRS Toolchain v1.30)
	$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c halt -c "program $(BUILD)/$(TARGET)" -c exit 		   # OpenOCD-wch 0.11 (MRS Toolchain v1.40)
	
	#$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c halt -c "verify_image $(BUILD)/$(TARGET)" -c exit
	
	#$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c "reset" -c exit || exit 0					   # OpenOCD-wch 0.10 (MRS Toolchain v1.30)
	$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c wlink_reset_resume -c exit || exit 0					   # OpenOCD-wch 0.11 (MRS Toolchain v1.40)
	
	#miniterm.py - 115200 --raw

include $(TOP)/py/mkrules.mk
