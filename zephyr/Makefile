#
# This is the main Makefile, which uses MicroPython build system,
# but Zephyr arch-specific toolchain and target-specific flags.
# This Makefile builds MicroPython as a library, and then calls
# recursively Makefile.zephyr to build complete application binary
# using Zephyr build system.
#

BOARD ?= qemu_x86
ifeq ($(MAKECMDGOALS), minimal)
# For minimal, CONF_FILE must be overriden early due to $(Z_EXPORTS) target
CONF_FILE = prj_minimal.conf
else
CONF_FILE = prj.conf
endif
# Zephyr 1.5.0
#OUTDIR_PREFIX =
# Zephyr 1.6.0
OUTDIR_PREFIX = $(BOARD)

# Default heap size is 16KB, which is on conservative side, to let
# it build for smaller boards, but it won't be enough for larger
# applications, and will need to be increased.
MICROPY_HEAP_SIZE = 16384
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
	modutime.c \
	modmachine.c \
	machine_pin.c \
	uart_core.c \
	lib/utils/stdout_helpers.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/utils/interrupt_char.c \
	lib/mp-readline/readline.c \
	$(SRC_MOD)

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

CFLAGS = $(KBUILD_CFLAGS) $(NOSTDINC_FLAGS) $(ZEPHYRINCLUDE) \
	 -std=gnu99 -fomit-frame-pointer -DNDEBUG -DMICROPY_HEAP_SIZE=$(MICROPY_HEAP_SIZE) $(CFLAGS_EXTRA) $(INC)

include ../py/mkrules.mk

$(Z_EXPORTS): $(CONF_FILE)
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) CONF_FILE=$(CONF_FILE) initconfig outputexports

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
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) CONF_FILE=$(CONF_FILE) $@

$(LIBMICROPYTHON): $(Z_SYSGEN_H)
build/genhdr/qstr.i.last: $(Z_SYSGEN_H)

$(Z_SYSGEN_H):
	rm -f $(LIBMICROPYTHON)
	-$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) CONF_FILE=$(CONF_FILE)

minimal:
	$(MAKE) BOARD=$(BOARD) CONF_FILE=prj_minimal.conf CFLAGS_EXTRA='-DMP_CONFIGFILE="<mpconfigport_minimal.h>"' FROZEN_DIR=

qemu-minimal:
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) CONF_FILE=prj_minimal.conf QEMU_NET=0 run

# Clean Zephyr things too
clean: z_clean

z_clean:
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) clean

.PHONY: prj.conf
prj.conf: prj_base.conf
	cat $< >$@
	if [ -f prj_$(BOARD).conf ]; then cat prj_$(BOARD).conf >>$@; fi
