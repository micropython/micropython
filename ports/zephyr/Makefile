#
# This is the main Makefile, which uses MicroPython build system,
# but Zephyr arch-specific toolchain and target-specific flags.
# This Makefile builds MicroPython as a library, and then calls
# recursively Makefile.zephyr to build complete application binary
# using Zephyr build system.
#
# To build a "minimal" configuration, use "make-minimal" wrapper.

BOARD ?= qemu_x86
CONF_FILE = prj_$(BOARD)_merged.conf
OUTDIR_PREFIX = $(BOARD)

# Default heap size is 16KB, which is on conservative side, to let
# it build for smaller boards, but it won't be enough for larger
# applications, and will need to be increased.
MICROPY_HEAP_SIZE = 16384
FROZEN_DIR = scripts

# Default target
all:

include ../../py/mkenv.mk
include $(TOP)/py/py.mk

# Zephyr (generated) config files - must be defined before include below
Z_EXPORTS = outdir/$(OUTDIR_PREFIX)/Makefile.export
ifneq ($(MAKECMDGOALS), clean)
include $(Z_EXPORTS)
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(ZEPHYR_BASE)/net/ip
INC += -I$(ZEPHYR_BASE)/net/ip/contiki
INC += -I$(ZEPHYR_BASE)/net/ip/contiki/os

SRC_C = main.c \
	help.c \
	modusocket.c \
	modutime.c \
	modzephyr.c \
	modzsensor.c \
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

CFLAGS = $(Z_CFLAGS) \
	 -std=gnu99 -fomit-frame-pointer -DNDEBUG -DMICROPY_HEAP_SIZE=$(MICROPY_HEAP_SIZE) $(CFLAGS_EXTRA) $(INC)

include $(TOP)/py/mkrules.mk

GENERIC_TARGETS = all zephyr run qemu qemugdb flash debug debugserver
KCONFIG_TARGETS = \
	initconfig config nconfig menuconfig xconfig gconfig \
	oldconfig silentoldconfig defconfig savedefconfig \
	allnoconfig allyesconfig alldefconfig randconfig \
	listnewconfig olddefconfig
CLEAN_TARGETS = pristine mrproper

$(GENERIC_TARGETS): $(LIBMICROPYTHON)
$(CLEAN_TARGETS):  clean

$(GENERIC_TARGETS) $(KCONFIG_TARGETS) $(CLEAN_TARGETS):
	$(MAKE) -C outdir/$(BOARD) $@

$(LIBMICROPYTHON): | $(Z_EXPORTS)
build/genhdr/qstr.i.last: | $(Z_EXPORTS)

# If we recreate libmicropython, also cause zephyr.bin relink
LIBMICROPYTHON_EXTRA_CMD = -$(RM) -f outdir/$(OUTDIR_PREFIX)/zephyr.lnk

# MicroPython's global clean cleans everything, fast
CLEAN_EXTRA = outdir libmicropython.a prj_*_merged.conf

# Clean Zephyr things in Zephyr way
z_clean:
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) clean

# This rule is for prj_$(BOARD)_merged.conf, not $(CONF_FILE), which
# can be overriden.
# prj_$(BOARD).conf is optional, that's why it's resolved with $(wildcard)
# function.
prj_$(BOARD)_merged.conf: prj_base.conf $(wildcard prj_$(BOARD).conf)
	$(PYTHON) makeprj.py prj_base.conf prj_$(BOARD).conf $@

test:
	cd $(TOP)/tests && ./run-tests --target minimal --device "execpty:make -C ../ports/zephyr run BOARD=$(BOARD) QEMU_PTY=1"

cmake: outdir/$(BOARD)/Makefile

outdir/$(BOARD)/Makefile: $(CONF_FILE)
	mkdir -p outdir/$(BOARD) && cmake -DBOARD=$(BOARD) -DCONF_FILE=$(CONF_FILE) -Boutdir/$(BOARD) -H.

$(Z_EXPORTS): outdir/$(BOARD)/Makefile
	make --no-print-directory -C outdir/$(BOARD) outputexports CMAKE_COMMAND=: >$@
	make -C outdir/$(BOARD) syscall_macros_h_target syscall_list_h_target
