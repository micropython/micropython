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

# Use the default frozen manifest, variants may override this.
FROZEN_MANIFEST ?= variants/manifest.py

# Qstr definitions (must come before including py.mk).
QSTR_DEFS = qstrdefsport.h

# Include py core make definitions.
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

################################################################################
# Project specific settings and compiler/linker flags.

CC = emcc
LD = emcc
NODE ?= node
TERSER ?= npx terser

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(VARIANT_DIR)

# Overridable by variants: the experimental JSPI variant builds with
# SUPPORT_LONGJMP=wasm (it needs a pure-Wasm call stack; see variants/jspi), and
# CSTD stays overridable for variants that require a GNU C mode.
CSTD ?= c99
SUPPORT_LONGJMP ?= emscripten

CFLAGS += -std=$(CSTD) -Wall -Werror -Wdouble-promotion -Wfloat-conversion
CFLAGS += -Os -DNDEBUG
CFLAGS += $(INC)

EXPORTED_FUNCTIONS_EXTRA += ,\
	_mp_js_do_exec,\
	_mp_js_do_exec_async,\
	_mp_js_do_import,\
	_mp_js_register_js_module,\
	_proxy_c_free_obj,\
	_proxy_c_init,\
	_proxy_c_to_js_call,\
	_proxy_c_to_js_delete_attr,\
	_proxy_c_to_js_dir,\
	_proxy_c_to_js_get_iter,\
	_proxy_c_to_js_get_type_and_data,\
	_proxy_c_to_js_has_attr,\
	_proxy_c_to_js_iternext,\
	_proxy_c_to_js_lookup_attr,\
	_proxy_c_to_js_resume,\
	_proxy_c_to_js_store_attr,\
	_proxy_convert_mp_to_js_obj_cside

EXPORTED_RUNTIME_METHODS_EXTRA += ,\
	HEAPU8,\
	PATH,\
	PATH_FS,\
	UTF8ToString,\
	getValue,\
	lengthBytesUTF8,\
	setValue,\
	stringToUTF8

JSFLAGS += -s EXPORTED_FUNCTIONS="\
	_free,\
	_malloc,\
	_mp_js_init,\
	_mp_js_repl_init,\
	_mp_js_repl_process_char,\
	_mp_hal_get_interrupt_char,\
	_mp_sched_keyboard_interrupt$(EXPORTED_FUNCTIONS_EXTRA)"
JSFLAGS += -s EXPORTED_RUNTIME_METHODS="\
	ccall,\
	cwrap,\
	FS$(EXPORTED_RUNTIME_METHODS_EXTRA)"
JSFLAGS += --js-library library.js
JSFLAGS += -s SUPPORT_LONGJMP=$(SUPPORT_LONGJMP)
JSFLAGS += -s MODULARIZE -s EXPORT_NAME=_createMicroPythonModule

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
	lexer_dedent.c \
	main.c \
	modjs.c \
	modjsffi.c \
	mphalport.c \
	objjsproxy.c \
	proxy_c.c \

# List of sources for qstr extraction.
SRC_QSTR += $(SRC_C) $(SRC_SHARED)

SRC_JS += \
	api.js \
	objpyproxy.js \
	proxy_js.js \

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

################################################################################
# Main targets.

.PHONY: all repl min test test//% test_min test_async

all: $(BUILD)/micropython.mjs

# LINK_DEPS lets a variant add link-time inputs (e.g. asyncify-fast's
# ASYNCIFY_ADD=@asyncify_add.txt) so editing them forces a relink.
$(BUILD)/micropython.mjs: $(OBJ) library.js $(SRC_JS) $(LINK_DEPS)
	$(ECHO) "LINK $@"
	$(Q)emcc $(LDFLAGS) -o $@ $(OBJ) $(JSFLAGS)
	$(Q)cat $(SRC_JS) >> $@

$(BUILD)/micropython.min.mjs: $(BUILD)/micropython.mjs
	$(TERSER) $< --compress --module -o $@

repl: $(BUILD)/micropython.mjs
	$(NODE) $<

min: $(BUILD)/micropython.min.mjs

test: $(BUILD)/micropython.mjs $(TOP)/tests/run-tests.py
	cd $(TOP)/tests && MICROPY_MICROPYTHON_MJS=../ports/webassembly/$< ./run-tests.py -t webassembly

test//%: $(BUILD)/micropython.mjs $(TOP)/tests/run-tests.py
	cd $(TOP)/tests && MICROPY_MICROPYTHON_MJS=../ports/webassembly/$< ./run-tests.py -t webassembly -i "$*"

test_min: $(BUILD)/micropython.min.mjs $(TOP)/tests/run-tests.py
	cd $(TOP)/tests && MICROPY_MICROPYTHON_MJS=../ports/webassembly/$< ./run-tests.py -t webassembly

# The async-only fixtures exercise features (cooperative yield, in-place stop,
# bounded GC, run_sync) that only work on a suspend-capable build; they SKIP on
# any other. Run just these, rather than the whole suite, against an asyncify or
# jspi build. Override ASYNC_TESTS to point at a different fixture (subset).
ASYNC_TESTS ?= ports/webassembly/async_*.mjs

test_async: $(BUILD)/micropython.mjs $(TOP)/tests/run-tests.py
	cd $(TOP)/tests && MICROPY_MICROPYTHON_MJS=../ports/webassembly/$< ./run-tests.py -t webassembly $(ASYNC_TESTS)

################################################################################
# Remaining make rules.

include $(TOP)/py/mkrules.mk
