# Select the variant to build for:
ifdef VARIANT_DIR
# Custom variant path - remove trailing slash and get the final component of
# the path as the variant name.
VARIANT ?= $(notdir $(VARIANT_DIR:/=))
else
# If not given on the command line, then default to standard.
VARIANT ?= standard
VARIANT_DIR ?= variants/$(VARIANT)
endif

ifeq ($(wildcard $(VARIANT_DIR)/.),)
$(error Invalid VARIANT specified: $(VARIANT_DIR))
endif

# If the build directory is not given, make it reflect the variant name.
BUILD ?= build-$(VARIANT)

include ../../py/mkenv.mk
-include mpconfigport.mk
include $(VARIANT_DIR)/mpconfigvariant.mk

# Use the default frozen manifest, variants may override this.
FROZEN_MANIFEST ?= variants/manifest.py

# This should be configured by the mpconfigvariant.mk
PROG ?= micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS += qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES += $(VARIANT_DIR)/mpconfigvariant.h

# OS name, for simple autoconfig
UNAME_S := $(shell uname -s)

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

GIT_SUBMODULES += lib/berkeley-db-1.xx

INC +=  -I.
INC +=  -I$(TOP)
INC += -I$(BUILD)

# compiler settings
CWARN = -Wall -Werror
CWARN += -Wextra -Wno-unused-parameter -Wpointer-arith -Wdouble-promotion -Wfloat-conversion
CFLAGS += $(INC) $(CWARN) -std=gnu99 -DUNIX $(COPT) -I$(VARIANT_DIR) $(CFLAGS_EXTRA)

# Force the use of 64-bits for file sizes in C library functions on 32-bit platforms.
# This option has no effect on 64-bit builds.
CFLAGS += -D_FILE_OFFSET_BITS=64

# Debugging/Optimization
ifdef DEBUG
COPT ?= -Og
else
COPT ?= -Os
COPT += -DNDEBUG
endif

# Remove unused sections.
COPT += -fdata-sections -ffunction-sections

# Note: Symbols and debug information will still be stripped from the final binary
# unless "DEBUG=1" or "STRIP=" is passed to make, see README.md for details.
CFLAGS += -g

ifndef DEBUG
# _FORTIFY_SOURCE is a feature in gcc/glibc which is intended to provide extra
# security for detecting buffer overflows. Some distros (Ubuntu at the very least)
# have it enabled by default.
#
# gcc already optimizes some printf calls to call puts and/or putchar. When
# _FORTIFY_SOURCE is enabled and compiling with -O1 or greater, then some
# printf calls will also be optimized to call __printf_chk (in glibc). Any
# printfs which get redirected to __printf_chk are then no longer synchronized
# with printfs that go through mp_printf.
#
# In MicroPython, we don't want to use the runtime library's printf but rather
# go through mp_printf, so that stdout is properly tied into streams, etc.
# This means that we either need to turn off _FORTIFY_SOURCE or provide our
# own implementation of __printf_chk. We've chosen to turn off _FORTIFY_SOURCE.
# It should also be noted that the use of printf in MicroPython is typically
# quite limited anyways (primarily for debug and some error reporting, etc
# in the unix version).
#
# Information about _FORTIFY_SOURCE seems to be rather scarce. The best I could
# find was this: https://securityblog.redhat.com/2014/03/26/fortify-and-you/
# Original patchset was introduced by
# https://gcc.gnu.org/ml/gcc-patches/2004-09/msg02055.html .
#
# Turning off _FORTIFY_SOURCE is only required when compiling with -O1 or greater
CFLAGS += -U _FORTIFY_SOURCE
endif

# On OSX, 'gcc' is a symlink to clang unless a real gcc is installed.
# The unix port of MicroPython on OSX must be compiled with clang,
# while cross-compile ports require gcc, so we test here for OSX and
# if necessary override the value of 'CC' set in py/mkenv.mk
ifeq ($(UNAME_S),Darwin)
ifeq ($(MICROPY_FORCE_32BIT),1)
CC = clang -m32
else
CC = clang
endif
# Use clang syntax for map file
LDFLAGS_ARCH = -Wl,-map,$@.map -Wl,-dead_strip
else
# Use gcc syntax for map file
LDFLAGS_ARCH = -Wl,-Map=$@.map,--cref -Wl,--gc-sections
endif
LDFLAGS += $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

# Flags to link with pthread library
LIBPTHREAD = -lpthread

ifeq ($(MICROPY_FORCE_32BIT),1)
# Note: you may need to install i386 versions of dependency packages,
# starting with linux-libc-dev:i386
ifeq ($(MICROPY_PY_FFI),1)
ifeq ($(UNAME_S),Linux)
CFLAGS += -I/usr/include/i686-linux-gnu
endif
endif
endif

ifeq ($(MICROPY_USE_READLINE),1)
INC += -I$(TOP)/shared/readline
CFLAGS += -DMICROPY_USE_READLINE=1
SHARED_SRC_C_EXTRA += readline/readline.c
endif
ifeq ($(MICROPY_PY_TERMIOS),1)
CFLAGS += -DMICROPY_PY_TERMIOS=1
endif
ifeq ($(MICROPY_PY_SOCKET),1)
CFLAGS += -DMICROPY_PY_SOCKET=1
endif
ifeq ($(MICROPY_PY_THREAD),1)
CFLAGS += -DMICROPY_PY_THREAD=1 -DMICROPY_PY_THREAD_GIL=0
LDFLAGS += $(LIBPTHREAD)
endif

ifeq ($(MICROPY_PY_SSL),1)
ifeq ($(MICROPY_SSL_AXTLS),1)

endif
endif

# If the variant enables it, enable modbluetooth.
ifeq ($(MICROPY_PY_BLUETOOTH),1)
ifeq ($(MICROPY_BLUETOOTH_BTSTACK),1)
HAVE_LIBUSB := $(shell (which pkg-config > /dev/null && pkg-config --exists libusb-1.0) 2>/dev/null && echo '1')

# Figure out which BTstack transport to use.
ifeq ($(HAVE_LIBUSB),1)
# Default to btstack-over-usb.
MICROPY_BLUETOOTH_BTSTACK_USB ?= 1
else
# Fallback to HCI controller via a H4 UART (e.g. Zephyr on nRF) over a /dev/tty serial port.
MICROPY_BLUETOOTH_BTSTACK_H4 ?= 1
endif

SRC_BTSTACK_C += lib/btstack/platform/embedded/btstack_run_loop_embedded.c
endif
endif

ifeq ($(MICROPY_PY_FFI),1)

ifeq ($(MICROPY_STANDALONE),1)
# Build libffi from source.
GIT_SUBMODULES += lib/libffi
DEPLIBS += libffi
LIBFFI_CFLAGS := -I$(shell ls -1d $(BUILD)/lib/libffi/include)
 ifeq ($(MICROPY_FORCE_32BIT),1)
  LIBFFI_LDFLAGS = $(BUILD)/lib/libffi/out/lib32/libffi.a
 else
  LIBFFI_LDFLAGS = $(BUILD)/lib/libffi/out/lib/libffi.a
 endif
else
# Use system version of libffi.
LIBFFI_CFLAGS := $(shell pkg-config --cflags libffi)
LIBFFI_LDFLAGS := $(shell pkg-config --libs libffi)
endif

ifeq ($(UNAME_S),Linux)
LIBFFI_LDFLAGS += -ldl
endif

CFLAGS += $(LIBFFI_CFLAGS) -DMICROPY_PY_FFI=1
LDFLAGS += $(LIBFFI_LDFLAGS)
endif

ifeq ($(MICROPY_PY_JNI),1)
# Path for 64-bit OpenJDK, should be adjusted for other JDKs
CFLAGS += -I/usr/lib/jvm/java-7-openjdk-amd64/include -DMICROPY_PY_JNI=1
endif

# source files
SRC_C += \
	main.c \
	gccollect.c \
	unix_mphal.c \
	mpthreadport.c \
	input.c \
	alloc.c \
	fatfs_port.c \
	mpbthciport.c \
	mpbtstackport_common.c \
	mpbtstackport_h4.c \
	mpbtstackport_usb.c \
	mpnimbleport.c \
	modtermios.c \
	modsocket.c \
	modffi.c \
	modjni.c \
	$(wildcard $(VARIANT_DIR)/*.c)

SHARED_SRC_C += $(addprefix shared/,\
	runtime/gchelper_generic.c \
	timeutils/timeutils.c \
	$(SHARED_SRC_C_EXTRA) \
	)

SRC_CXX += \

OBJ = $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_CXX) $(SHARED_SRC_C)

ifneq ($(FROZEN_MANIFEST),)
CFLAGS += -DMPZ_DIG_SIZE=16 # force 16 bits to work on both 32 and 64 bit archs
endif

CXXFLAGS += $(filter-out -Wmissing-prototypes -Wold-style-definition -std=gnu99,$(CFLAGS) $(CXXFLAGS_MOD))

ifeq ($(MICROPY_FORCE_32BIT),1)
RUN_TESTS_MPY_CROSS_FLAGS = --mpy-cross-flags='-march=x86'
endif

ifeq ($(CROSS_COMPILE),arm-linux-gnueabi-)
# Force disable error text compression when compiling for ARM as the compiler
# cannot optimise out the giant strcmp list generated for MP_MATCH_COMPRESSED.
# Checked on:
# arm-linux-gnueabi-gcc (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0
# arm-linux-gnueabi-gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
# See https://github.com/micropython/micropython/pull/7659 for details.
$(info Detected arm-linux-gnueabi-gcc. Disabling error message compression.)
MICROPY_ROM_TEXT_COMPRESSION = 0
endif

include $(TOP)/py/mkrules.mk

.PHONY: test test_full_no_native test_full

test: $(BUILD)/$(PROG) $(TOP)/tests/run-tests.py
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(BUILD)/$(PROG) ./run-tests.py

test_full_no_native: $(BUILD)/$(PROG) $(TOP)/tests/run-tests.py
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(BUILD)/$(PROG) ./run-tests.py
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(BUILD)/$(PROG) ./run-tests.py -d thread
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(BUILD)/$(PROG) ./run-tests.py --via-mpy $(RUN_TESTS_MPY_CROSS_FLAGS) -d basics float micropython
	cat $(TOP)/tests/basics/0prelim.py | ./$(BUILD)/$(PROG) | grep -q 'abc'

test_full: $(BUILD)/$(PROG) $(TOP)/tests/run-tests.py test_full_no_native
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(BUILD)/$(PROG) ./run-tests.py --emit native
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(BUILD)/$(PROG) ./run-tests.py --via-mpy $(RUN_TESTS_MPY_CROSS_FLAGS) --emit native -d basics float micropython

test_gcov: test_full
	gcov -o $(BUILD)/py $(TOP)/py/*.c
	gcov -o $(BUILD)/extmod $(TOP)/extmod/*.c

# Value of configure's --host= option (required for cross-compilation).
# Deduce it from CROSS_COMPILE by default, but can be overridden.
ifneq ($(CROSS_COMPILE),)
CROSS_COMPILE_HOST = --host=$(patsubst %-,%,$(CROSS_COMPILE))
else
CROSS_COMPILE_HOST =
endif

deplibs: $(DEPLIBS)

libffi: $(BUILD)/lib/libffi/include/ffi.h

$(TOP)/lib/libffi/configure: $(TOP)/lib/libffi/autogen.sh
	cd $(TOP)/lib/libffi; ./autogen.sh

# install-exec-recursive & install-data-am targets are used to avoid building
# docs and depending on makeinfo
$(BUILD)/lib/libffi/include/ffi.h: $(TOP)/lib/libffi/configure
	mkdir -p $(BUILD)/lib/libffi; cd $(BUILD)/lib/libffi; \
	$(abspath $(TOP))/lib/libffi/configure $(CROSS_COMPILE_HOST) --prefix=$$PWD/out --disable-shared --disable-structs CC="$(CC)" CXX="$(CXX)" LD="$(LD)" CFLAGS="-Os -fomit-frame-pointer -fstrict-aliasing -ffast-math -fno-exceptions"; \
	$(MAKE) install-exec-recursive; $(MAKE) -C include install-data-am

PREFIX = /usr/local
BINDIR = $(DESTDIR)$(PREFIX)/bin

install: $(BUILD)/$(PROG)
	install -d $(BINDIR)
	install $(BUILD)/$(PROG) $(BINDIR)/$(PROG)

uninstall:
	-rm $(BINDIR)/$(PROG)
