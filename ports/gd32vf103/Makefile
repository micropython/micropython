include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

GIT_SUBMODULES = ports/gd32vf103/gd32vf103inator

CROSS_COMPILE = riscv64-unknown-elf-
OBJDUMP = $(CROSS_COMPILE)objdump

DFU_UTIL   = dfu-util
DFU_DEVICE = 1d50:613e

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -Igd32vf103inator/include
INC += -Igd32vf103inator/std

ARCH       = rv32imac
ABI        = ilp32
CODEMODEL  = medlow

BOOTLOADER = 4*1024
FLASH_SIZE = 128*1024
RAM_SIZE   = 32*1024

CORECLOCK  = 96000000
HXTAL      = 8000000

ARCHFLAGS  = -march=$(ARCH) -mabi=$(ABI) -mcmodel=$(CODEMODEL) -fno-pie
WARNINGS   = -Wall -Wextra -Wshadow -Wformat=2 -Wformat-truncation=2 -Wno-unused-parameter
DIMENSIONS = -DBOOTLOADER=$(BOOTLOADER) -DFLASH_SIZE=$(FLASH_SIZE) -DRAM_SIZE=$(RAM_SIZE)
CLOCKS     = -DHXTAL=$(HXTAL) -DCORECLOCK=$(CORECLOCK)

CFLAGS    = $(COPT) -std=c99 $(ARCHFLAGS) $(WARNINGS)
CFLAGS   += $(INC) $(DIMENSIONS) $(CLOCKS) -D_LIBC_LIMITS_H_
CFLAGS   += -ffreestanding #-ftls-model=local-exec
LDFLAGS   = $(COPT) $(ARCHFLAGS) -static -nostdlib -Tgd32vf103inator/gd32vf103.ld
LDFLAGS  += -Wl,-O1,--gc-sections,--relax,--build-id=none,-Map=$@.map,--cref

CSUPEROPT = -Os # save some code space

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
COPT    = -Os -ggdb
else
COPT    = -Os -flto -DNDEBUG
CFLAGS += -fno-common -ffunction-sections -fdata-sections
endif


LIBS = -lgcc

SRC_C = \
	main.c \
	usbacm.c \
	mphalport.c \
	modutime.c \
	modmachine.c \
	pin.c \
	gd32vf103inator/lib/rcu.c \
	gd32vf103inator/lib/eclic.c \
	gd32vf103inator/lib/mtimer.c \
	gd32vf103inator/lib/gpio.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/mp-readline/readline.c \
	lib/libc/string0.c

# List of sources for qstr extraction
SRC_QSTR += modutime.c pin.c modmachine.c

OBJ = $(BUILD)/gd32vf103inator/start.o \
      $(PY_O) \
      $(patsubst %.c,$(BUILD)/%.o,$(SRC_C))

.PHONY: all dump dfu romdfu

all: $(BUILD)/firmware.bin

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "OBJCOPY $@"
	$(Q)$(OBJCOPY) -O binary $^ $@

dump: $(BUILD)/firmware.elf
	$(OBJDUMP) -x -d -j .init -j .text -j .data $< | $(PAGER)

dfu: $(BUILD)/firmware.bin
	$(Q)$(DFU_UTIL) -d $(DFU_DEVICE) -D $< -R

romdfu: $(BUILD)/firmware.bin
	$(Q)$(DFU_UTIL) -d 28e9:0189 -a 0 --dfuse-address 0x08000000:leave -D $<

include $(TOP)/py/mkrules.mk
