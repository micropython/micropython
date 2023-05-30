################################################################################
# Initial setup of Makefile environment.

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
include $(VARIANT_DIR)/mpconfigvariant.mk

# Qstr definitions (must come before including py.mk).
QSTR_DEFS = qstrdefsport.h

# Include py core make definitions.
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

################################################################################
# Project specific settings and compiler/linker flags.

CC = emcc
LD = emcc
TERSER ?= npx terser

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(VARIANT_DIR)

CFLAGS += -std=c99 -Wall -Werror -Wdouble-promotion -Wfloat-conversion
CFLAGS += -Os -DNDEBUG
CFLAGS += $(INC)

JSFLAGS += -s EXPORTED_FUNCTIONS="\
	_mp_js_init,\
	_mp_js_init_repl,\
	_mp_js_do_str,\
	_mp_js_process_char,\
	_mp_hal_get_interrupt_char,\
	_mp_sched_keyboard_interrupt$(EXPORTED_FUNCTIONS_EXTRA)"
JSFLAGS += -s EXPORTED_RUNTIME_METHODS="\
	ccall,\
	cwrap,\
	FS$(EXPORTED_RUNTIME_METHODS_EXTRA)"
JSFLAGS += --js-library library.js
JSFLAGS += -s SUPPORT_LONGJMP=emscripten

################################################################################
# Source files and libraries.

SRC_SHARED = $(addprefix shared/,\
	runtime/interrupt_char.c \
	runtime/stdout_helpers.c \
	runtime/pyexec.c \
	readline/readline.c \
	timeutils/timeutils.c \
	)

SRC_C += \
	main.c \
	mphalport.c \

# List of sources for qstr extraction.
SRC_QSTR += $(SRC_C) $(SRC_SHARED)

SRC_JS ?= wrapper.js

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

################################################################################
# Main targets.

.PHONY: all min test

all: $(BUILD)/micropython.js

$(BUILD)/micropython.js: $(OBJ) library.js $(SRC_JS)
	$(ECHO) "LINK $@"
	$(Q)emcc $(LDFLAGS) -o $@ $(OBJ) $(JSFLAGS)
	$(Q)cat $(SRC_JS) >> $@

$(BUILD)/micropython.min.js: $(BUILD)/micropython.js
	$(TERSER) $< --compress --module -o $@

min: $(BUILD)/micropython.min.js

test: $(BUILD)/micropython.js $(TOP)/tests/run-tests.py
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../ports/webassembly/node_run.sh ./run-tests.py -j1

################################################################################
# Remaining make rules.

include $(TOP)/py/mkrules.mk
