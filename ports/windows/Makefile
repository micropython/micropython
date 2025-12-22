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

FROZEN_MANIFEST ?= variants/manifest.py

# Define main target
PROG ?= micropython

# qstr definitions (must come before including py.mk)
QSTR_DEFS += ../unix/qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES += $(VARIANT_DIR)/mpconfigvariant.h

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(VARIANT_DIR)

# compiler settings
CFLAGS += $(INC) -Wall -Wpointer-arith -Wdouble-promotion -Werror -std=gnu99 -DUNIX -D__USE_MINGW_ANSI_STDIO=1 $(COPT) $(CFLAGS_EXTRA)
LDFLAGS += -lm -lbcrypt $(LDFLAGS_EXTRA)

# Force the use of 64-bits for file sizes in C library functions on 32-bit platforms.
# This option has no effect on 64-bit builds.
CFLAGS += -D_FILE_OFFSET_BITS=64

# Force the use of 64-bits for time_t in C library functions on 32-bit platforms.
# This option has no effect on 64-bit builds.
CFLAGS += -D_TIME_BITS=64

# Debugging/Optimization
ifdef DEBUG
CFLAGS += -g
COPT = -O0
else
COPT = -Os #-DNDEBUG
endif

# source files
SRC_C = \
	shared/libc/printf.c \
	shared/runtime/gchelper_generic.c \
	ports/unix/main.c \
	ports/unix/input.c \
	ports/unix/gccollect.c \
	ports/unix/mpbtstackport_common.c \
	ports/unix/mpbtstackport_h4.c \
	ports/unix/mpbtstackport_usb.c \
	ports/unix/mpnimbleport.c \
	mpbthciport.c \
	windows_mphal.c \
	realpath.c \
	init.c \
	fmode.c \
	$(wildcard $(VARIANT_DIR)/*.c)

SHARED_SRC_C += $(addprefix shared/,\
	$(SHARED_SRC_C_EXTRA) \
	)

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(EXTMOD_SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))

ifeq ($(MICROPY_USE_READLINE),1)
CFLAGS += -DMICROPY_USE_READLINE=1
SRC_C += shared/readline/readline.c
SRC_C += shared/runtime/pyexec.c
endif

LIB += -lws2_32

# List of sources for qstr extraction
SRC_QSTR += $(SRC_C) $(SRC_CXX) $(SHARED_SRC_C) $(EXTMOD_SRC_C)

ifneq ($(FROZEN_MANIFEST),)
CFLAGS += -DMPZ_DIG_SIZE=16
endif

ifeq ($(shell $(CC) -dumpmachine),i686-w64-mingw32)
# GCC disables the SSE instruction set by default on i366 targets and we have
# to specify all three of these options to enable it.
# https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html (see -mfpmath=unit section)
# Enabling the SSE instruction set is necessary to get correct rounding of floating points.
# https://lemire.me/blog/2020/06/26/gcc-not-nearest
CFLAGS += -msse -mfpmath=sse -march=pentium4
endif

# If the variant enables it, enable modbluetooth.
ifeq ($(MICROPY_PY_BLUETOOTH),1)

# Only one stack can be enabled.
ifeq ($(MICROPY_BLUETOOTH_NIMBLE),1)
ifeq ($(MICROPY_BLUETOOTH_BTSTACK),1)
$(error Cannot enable both NimBLE and BTstack at the same time)
endif
endif

# Default to btstack, but a variant (or make command line) can set NimBLE
# explicitly (which is always via H4 UART).
ifneq ($(MICROPY_BLUETOOTH_NIMBLE),1)
ifneq ($(MICROPY_BLUETOOTH_BTSTACK),1)
MICROPY_BLUETOOTH_BTSTACK ?= 1
endif
endif

CFLAGS += -mno-ms-bitfields
CFLAGS += -DMICROPY_PY_BLUETOOTH=1
CFLAGS += -DMICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE=1

ifeq ($(MICROPY_BLUETOOTH_BTSTACK),1)

# Figure out which BTstack transport to use.
ifeq ($(MICROPY_BLUETOOTH_BTSTACK_H4),1)
ifeq ($(MICROPY_BLUETOOTH_BTSTACK_USB),1)
$(error Cannot enable BTstack support for USB and H4 UART at the same time)
endif
else
ifeq ($(HAVE_LIBUSB),1)
# Default to btstack-over-usb.
MICROPY_BLUETOOTH_BTSTACK_USB ?= 1
else
# Fallback to HCI controller via a H4 UART (e.g. Zephyr on nRF) over a /dev/tty serial port.
MICROPY_BLUETOOTH_BTSTACK_H4 ?= 1
endif
endif

# BTstack is enabled.
GIT_SUBMODULES += lib/btstack
include $(TOP)/extmod/btstack/btstack.mk
SRC_BTSTACK += lib/btstack/platform/embedded/btstack_run_loop_embedded.c

else

# NimBLE is enabled.
GIT_SUBMODULES += lib/mynewt-nimble
CFLAGS += -DMICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS=1
include $(TOP)/extmod/nimble/nimble.mk

endif
endif

CXXFLAGS += $(filter-out -std=gnu99,$(CFLAGS))


include $(TOP)/py/mkrules.mk

.PHONY: test test_full

# Note for recent gcc versions like 13.2:
# - mingw64-x86_64 gcc builds will pass the math_domain_special test
# - mingw64-ucrt64 gcc builds will pass all of the below tests
RUN_TESTS_SKIP += -e math_fun -e float2int_double -e float_parse -e math_domain_special

test: $(BUILD)/$(PROG) $(TOP)/tests/run-tests.py
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(BUILD)/$(PROG) $(PYTHON) ./run-tests.py $(RUN_TESTS_SKIP)

test_full: test
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../$(DIRNAME)/$(BUILD)/$(PROG) $(PYTHON) ./run-tests.py --via-mpy $(RUN_TESTS_MPY_CROSS_FLAGS) $(RUN_TESTS_SKIP) -d basics float micropython

$(BUILD)/$(PROG): $(BUILD)/micropython.res

$(BUILD)/%.res: %.rc
	$(ECHO) "WINDRES $<"
	$(Q)$(WINDRES) $< -O coff -o $@
