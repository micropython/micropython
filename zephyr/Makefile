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

ARCH ?= x86
BOARD ?= qemu_x86
# Zephyr 1.5.0
#OUTDIR_PREFIX =
# Zephyr 1.6.0
OUTDIR_PREFIX = $(BOARD)

# Zephyr toolchain config is 2-pass, so included twice
include $(ZEPHYR_BASE)/scripts/Makefile.toolchain.$(ZEPHYR_GCC_VARIANT)
include $(ZEPHYR_BASE)/scripts/Makefile.toolchain.$(ZEPHYR_GCC_VARIANT)

CFLAGS_arm = -mthumb -mcpu=cortex-m3 -mabi=aapcs
CFLAGS_x86 = -fno-asynchronous-unwind-tables -ffreestanding -fno-stack-protector \
    -fno-omit-frame-pointer -mpreferred-stack-boundary=2 -mno-sse -march=pentium
CFLAGS_TARGET = $(CFLAGS_$(ARCH))

include ../py/mkenv.mk
include ../py/py.mk

INC += -I.
INC += -I..
INC += -I$(BUILD)
INC += -I$(ZEPHYR_BASE)/include -I$(ZEPHYR_BASE) \
    -Ioutdir/$(OUTDIR_PREFIX)/misc/generated/sysgen \
    -I$(dir $(Z_AUTOCONF_H))

SRC_C = main.c \
	uart_core.c \
	lib/utils/stdout_helpers.c \
	lib/utils/printf.c \
	lib/utils/pyexec.c \
	lib/mp-readline/readline.c \
	$(SRC_MOD)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

COPT = -Os -DNDEBUG -fdata-sections -ffunction-sections
CFLAGS = -std=gnu99 $(TOOLCHAIN_CFLAGS) $(INC) $(CFLAGS_MOD) $(COPT) $(CFLAGS_TARGET)

include ../py/mkrules.mk

all: $(LIBMICROPYTHON)
	$(MAKE) -f Makefile.zephyr BOARD=$(BOARD)

qemu: all
	$(MAKE) -f Makefile.zephyr qemu BOARD=$(BOARD)

Z_AUTOCONF_H = outdir/$(OUTDIR_PREFIX)/include/generated/autoconf.h

$(LIBMICROPYTHON): $(Z_AUTOCONF_H)
build/genhdr/qstr.i.last: $(Z_AUTOCONF_H)

$(Z_AUTOCONF_H):
	rm -f $(LIBMICROPYTHON)
	-$(MAKE) -f Makefile.zephyr BOARD=$(BOARD)

# Clean Zephyr things too
CLEAN_EXTRA = outdir
