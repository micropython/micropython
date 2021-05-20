#
# This is the main Makefile, which uses MicroPython build system,
# but Zephyr arch-specific toolchain and target-specific flags.
# This Makefile builds MicroPython as a library, and then calls
# recursively Makefile.zephyr to build complete application binary
# using Zephyr build system.
#
# To build a "minimal" configuration, use "make-minimal" wrapper.

BOARD ?= qemu_x86
OUTDIR_PREFIX = $(BOARD)

# Default heap size is 16KB, which is on conservative side, to let
# it build for smaller boards, but it won't be enough for larger
# applications, and will need to be increased.
MICROPY_HEAP_SIZE = 16384
FROZEN_DIR = scripts

MICROPY_VFS_FAT ?= 1
MICROPY_VFS_LFS1 ?= 0
MICROPY_VFS_LFS2 ?= 1

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
INC += -I$(ZEPHYR_BASE)/micropython
INC += -I$(ZEPHYR_BASE)/net/ip
INC += -I$(ZEPHYR_BASE)/net/ip/contiki
INC += -I$(ZEPHYR_BASE)/net/ip/contiki/os
INC += -I$(ZEPHYR_BASE)/kernel/include
INC += -I$(ZEPHYR_BASE)/arch/arm/include

SRC_C = main.c \
	help.c \
	moduos.c \
	modusocket.c \
	modutime.c \
	modzephyr.c \
	modzsensor.c \
	modmachine.c \
	machine_i2c.c \
	machine_pin.c \
	machine_uart.c \
	uart_core.c \
	zephyr_storage.c \
	lib/timeutils/timeutils.c \
	lib/utils/mpirq.c \
	lib/utils/stdout_helpers.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/utils/interrupt_char.c \
	lib/mp-readline/readline.c \
	$(SRC_MOD)

# lv binding
SRC_C +=  $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display_16bit.c \
    $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display_24bit.c \
    $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display_32bit.c \
    $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display.c \
    $(ZEPHYR_BASE)/lib/gui/lvgl/lvgl_display_mono.c \
    build/lvgl/lv_mpy.c

INC += -I$(ZEPHYR_BASE)
INC += -I$(TOP)/lib/lv_bindings
INC += -I$(TOP)/lib/lv_bindings/driver/zephyr
INC += -I$(ZEPHYR_BASE)/include
INC += -I$(ZEPHYR_BASE)/lib/gui/lvgl/
INC += -I$(TOP)/lib/lv_bindings/lvgl


# List of sources for qstr extraction
SRC_QSTR += $(SRC_C)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

CFLAGS = $(Z_CFLAGS) \
	 -std=gnu99 -fomit-frame-pointer -DNDEBUG -DMICROPY_HEAP_SIZE=$(MICROPY_HEAP_SIZE) $(CFLAGS_MOD) $(CFLAGS_EXTRA) $(INC)

include $(TOP)/py/mkrules.mk

GENERIC_TARGETS = all zephyr run qemu qemugdb flash debug debugserver \
	ram_report rom_report
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
CLEAN_EXTRA = outdir libmicropython.a

# Clean Zephyr things in Zephyr way
z_clean:
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD) clean

test:
	cd $(TOP)/tests && ./run-tests --target minimal --device "execpty:make -C ../ports/zephyr run BOARD=$(BOARD) QEMU_PTY=1"

cmake: outdir/$(BOARD)/Makefile

ifneq ($(CONF_FILE),)
CMAKE_MOD += -DCONF_FILE=$(CONF_FILE)
endif

outdir/$(BOARD)/Makefile:
	mkdir -p outdir/$(BOARD) && cmake -DBOARD=$(BOARD) $(CMAKE_MOD) -Boutdir/$(BOARD) -H.

$(Z_EXPORTS): outdir/$(BOARD)/Makefile
	make --no-print-directory -C outdir/$(BOARD) outputexports CMAKE_COMMAND=: >$@
	make -C outdir/$(BOARD) syscall_list_h_target kobj_types_h_target
