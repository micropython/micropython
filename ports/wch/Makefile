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
TARGET = firmware.elf
SRC_DIR = vendor/src/
INC_DIR = vendor/inc/

# Toolchain
CC = $(TOOLCHAIN_ROOT)riscv-none-embed-gcc
DB = $(TOOLCHAIN_ROOT)riscv-none-embed-gdb

# Project sources
SRC_C = \
    main.c \
	mphalport.c \
	shared/libc/string0.c \
    shared/readline/readline.c \
    shared/runtime/gchelper_generic.c \
    shared/runtime/pyexec.c \
    shared/runtime/stdout_helpers.c \
    
SRC_C += $(wildcard $(SRC_DIR)*.c) 

SRC_S += vendor/startup_ch32v30x_D8C.S

SRC_C += $(SRC_MOD)
SRC_CXX += $(SRC_MOD_CXX)
SRC_QSTR += $(SRC_MOD) $(SRC_MOD_CXX)

# Project includes
INCLUDES = -I. -I$(INC_DIR) -I$(BUILD) -I$(TOP)

# Linker Script
LD_SCRIPT = vendor/ch32v30x.ld

# Compiler Flags
CFLAGS  = -march=rv32imac -mabi=ilp32 -msmall-data-limit=8 -mno-save-restore -Os
CFLAGS += -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -g 
CFLAGS += -std=gnu99 
CFLAGS += $(INCLUDES)

# Linker Flags
LDFLAGS = -T $(LD_SCRIPT) -nostartfiles -Xlinker --gc-sections --specs=nano.specs --specs=nosys.specs

###############################################################################

OBJ = $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.S=.o))

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJ)
	$(ECHO) "LINK $@"        
	$(Q)$(CC) $(LDFLAGS) -o $@ $^
	$(Q)$(SIZE) $@

prog: $(BUILD)/$(TARGET)
	$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c halt -c "flash erase_sector wch_riscv 0 last " -c exit
	$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c halt -c "program $(BUILD)/$(TARGET) 0x08000000" -c exit
	#$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c halt -c "verify_image $(BUILD)/$(TARGET)" -c exit
	$(OPENOCD_ROOT)openocd -f $(OPENOCD_ROOT)wch-riscv.cfg -c init -c "reset" -c exit || exit 0
	#miniterm.py - 115200 --raw

include $(TOP)/py/mkrules.mk
