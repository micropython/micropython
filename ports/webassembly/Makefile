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
CMP = cmp

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(VARIANT_DIR)

CFLAGS += -std=c99 -Wall -Werror -Wdouble-promotion -Wfloat-conversion
CFLAGS += -Os -DNDEBUG
CFLAGS += $(INC)
CFLAGS += $(CFLAGS_EXTRA)

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
	_mp_js_repl_process_char_async,\
	_mp_hal_get_interrupt_char,\
	_mp_sched_keyboard_interrupt$(EXPORTED_FUNCTIONS_EXTRA)"
JSFLAGS += -s EXPORTED_RUNTIME_METHODS="\
	ccall,\
	cwrap,\
	FS$(EXPORTED_RUNTIME_METHODS_EXTRA)"
JSFLAGS += --js-library library.js
# Longjmp support mode; the browser variant overrides this to "wasm"
# because the JS invoke_* trampolines used by the emscripten mode are
# incompatible with JSPI stack suspension.
SUPPORT_LONGJMP ?= emscripten
CFLAGS += -s SUPPORT_LONGJMP=$(SUPPORT_LONGJMP)
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

.PHONY: all repl min test test//% test_min npm_pack publish

all: $(BUILD)/micropython.mjs

$(BUILD)/micropython.mjs: $(OBJ) library.js $(SRC_JS)
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

################################################################################
# Publishing to npm.

# A variant is publishable when it has an npm/ directory holding a
# package.json (with @VERSION@ where the version goes) and a README.md.
# The package holds the minified JS plus two wasm files: the variant as
# built, and a settrace sub-variant for debuggers.  Both builds share
# the same JS, which is checked, so one micropython.mjs serves either
# wasm.  The package version must be given with NPM_VERSION=... on the
# command line.  npm_pack builds the tarball for inspection, publish
# uploads it with the credentials of whoever runs it.

BUILD_NPM = $(BUILD)-npm
BUILD_SETTRACE = $(BUILD)-settrace

ifneq ($(filter npm_pack publish,$(MAKECMDGOALS)),)
ifeq ($(NPM_VERSION),)
$(error NPM_VERSION must be set, e.g. make NPM_VERSION=1.30.0 $(MAKECMDGOALS))
endif
endif

$(BUILD_SETTRACE)/micropython.min.mjs: FORCE
	$(Q)$(MAKE) VARIANT=$(VARIANT) BUILD=$(BUILD_SETTRACE) CFLAGS_EXTRA="$(CFLAGS_EXTRA) -DMICROPY_PY_SYS_SETTRACE=1" min

$(BUILD_NPM)/package.json: $(BUILD)/micropython.min.mjs $(BUILD_SETTRACE)/micropython.min.mjs $(VARIANT_DIR)/npm/package.json $(VARIANT_DIR)/npm/README.md FORCE
	$(ECHO) "Create $@"
	$(Q)$(CMP) $(BUILD)/micropython.min.mjs $(BUILD_SETTRACE)/micropython.min.mjs
	$(Q)$(MKDIR) -p $(BUILD_NPM)
	$(Q)$(CP) $(BUILD)/micropython.min.mjs $(BUILD_NPM)/micropython.mjs
	$(Q)$(CP) $(BUILD)/micropython.wasm $(BUILD_NPM)/micropython.wasm
	$(Q)$(CP) $(BUILD_SETTRACE)/micropython.wasm $(BUILD_NPM)/micropython-settrace.wasm
	$(Q)$(CP) $(VARIANT_DIR)/npm/README.md $(BUILD_NPM)/README.md
	$(Q)$(SED) 's/@VERSION@/$(NPM_VERSION)/' $(VARIANT_DIR)/npm/package.json > $@

npm_pack: $(BUILD_NPM)/package.json
	$(Q)cd $(BUILD_NPM) && npm pack

publish: $(BUILD_NPM)/package.json
	$(Q)cd $(BUILD_NPM) && npm publish --tag latest --access public

################################################################################
# Remaining make rules.

include $(TOP)/py/mkrules.mk
