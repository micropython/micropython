#
# This is main Makefile, which uses MicroPython build system, but
# Zephyr arch-specific toolchain (setup by Zephyr's Makefile.toolchain.*).
# Unfortunately, it's currently not possible to get target (as in: specific
# board to run on) specific compile-time options from Zephyr, so these must
# be set (duplicated) in this Makefile. Currently, these configured for
# ARM Cortex-M3. This Makefile builds MicroPython as a library, and then
# calls recursively Makefile.zephyr to build complete application using
# Zephyr build system.
#

BOARD ?= qemu_x86
# Zephyr 1.5.0
#OUTDIR_PREFIX =
# Zephyr 1.6.0
OUTDIR_PREFIX = $(BOARD)

FROZEN_DIR = scripts

# Zephyr (generated) config files - must be defined before include below
Z_SYSGEN_H = outdir/$(OUTDIR_PREFIX)/misc/generated/sysgen/sysgen.h
Z_EXPORTS = outdir/$(OUTDIR_PREFIX)/Makefile.export
include $(Z_EXPORTS)

include ../py/mkenv.mk
include ../py/py.mk

INC += -I.
INC += -I..
INC += -I$(BUILD)
INC += -I$(ZEPHYR_BASE)/net/ip
INC += -I$(ZEPHYR_BASE)/net/ip/contiki
INC += -I$(ZEPHYR_BASE)/net/ip/contiki/os

SRC_C = main.c \
	help.c \
	uart_core.c \
	lib/utils/stdout_helpers.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/utils/interrupt_char.c \
	lib/utils/pyhelp.c \
	lib/mp-readline/readline.c \
	$(BUILD)/frozen.c \
	$(SRC_MOD)

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

CFLAGS = $(KBUILD_CFLAGS) $(NOSTDINC_FLAGS) $(ZEPHYRINCLUDE) \
	 -std=gnu99 -DNDEBUG $(INC)

include ../py/mkrules.mk

$(Z_EXPORTS):
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) initconfig outputexports

GENERIC_TARGETS = all zephyr qemu qemugdb flash debug
KCONFIG_TARGETS = \
	initconfig config nconfig menuconfig xconfig gconfig \
	oldconfig silentoldconfig defconfig savedefconfig \
	allnoconfig allyesconfig alldefconfig randconfig \
	listnewconfig olddefconfig
CLEAN_TARGETS = pristine mrproper

$(GENERIC_TARGETS): $(LIBMICROPYTHON)
$(CLEAN_TARGETS):  clean

$(GENERIC_TARGETS) $(KCONFIG_TARGETS) $(CLEAN_TARGETS):
	$(RM) -f outdir/$(OUTDIR_PREFIX)/zephyr.lnk
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) $@

$(LIBMICROPYTHON): $(Z_SYSGEN_H)
build/genhdr/qstr.i.last: $(Z_SYSGEN_H)

$(Z_SYSGEN_H):
	rm -f $(LIBMICROPYTHON)
	-$(MAKE) -f Makefile.zephyr BOARD=$(BOARD)

# Clean Zephyr things too
clean: z_clean

z_clean:
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) clean
