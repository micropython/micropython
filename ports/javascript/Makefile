include ../../py/mkenv.mk

CROSS = 0

QSTR_DEFS = qstrdefsport.h

WASM_FILE_API = 1

include $(TOP)/py/py.mk

JSFLAGS = $(BOTH_CFLAGS) -s ASSERTIONS=2 -s FORCE_FILESYSTEM=1 -s USE_SDL=2 $(OPTIM) -s EXPORTED_FUNCTIONS="['_mp_js_init', '_mp_handle_pending', '_mp_js_init_repl', '_mp_js_do_str', '_mp_js_process_char', '_mp_hal_get_interrupt_char', '_mp_keyboard_interrupt']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" --memory-init-file 0 --js-library library.js

CC = emcc -g4 $(JSFLAGS)
LD = emcc -g4

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

BOTH_CFLAGS := -s WASM=0

CPP := clang -E -undef -include $(BUILD)/clang_predefs.h


ifdef WASM_FILE_API
	CPPFLAGS += -DWASM_FILE_API=1 
endif

ifdef EMSCRIPTEN
	CPP += -D__EMSCRIPTEN__
	CPP += --sysroot $(EMSCRIPTEN)/system
	CPP += $(addprefix -isystem, $(shell env LC_ALL=C $(CC) -E -x c++ /dev/null -v 2>&1 |sed -e '/^\#include <...>/,/^End of search/{ //!b };d'))
	CPP += $(CPPFLAGS)
endif

CFLAGS = -m32 -Wall $(INC) $(COPT) $(CPPFLAGS)
LDFLAGS = -m32 -Wl,-Map=$@.map,--cref -Wl,--gc-sections

OPTIM := -O2

CFLAGS += $(OPTIM) -DNDEBUG -D__EMSCRIPTEN__ -DLV_CONF_INCLUDE_SIMPLE
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += $(CFLAGS_MOD)

ifneq ($(FROZEN_DIR),)
# To use frozen source modules, put your .py files in a subdirectory (eg scripts/)
# and then invoke make with FROZEN_DIR=scripts (be sure to build from scratch).
CFLAGS += -DMICROPY_MODULE_FROZEN_STR
endif

ifneq ($(FROZEN_MPY_DIR),)
# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen (be sure to build from scratch).
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
endif

SRC_LIB = $(addprefix lib/,\
	utils/interrupt_char.c \
	utils/stdout_helpers.c \
	utils/pyexec.c \
	mp-readline/readline.c \
	)

SRC_C = \
	main.c \
	mphalport.c \
	modutime.c
	
ifdef WASM_FILE_API
	SRC_C += file.c wasm_file_api.c
endif	

SRC_C += $(SRC_MOD)

LIB_SRC_C = $(addprefix lib/,\
    lv_bindings/driver/SDL/SDL_monitor.c \
    lv_bindings/driver/SDL/SDL_mouse.c \
	lv_bindings/driver/SDL/modSDL.c \
	$(LIB_SRC_C_EXTRA) \
	timeutils/timeutils.c \
	)

OBJ = 
OBJ = $(PY_O) 
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))

# List of sources for qstr extraction
SRC_QSTR += $(BUILD)/clang_predefs.h $(SRC_C) $(LIB_SRC_C) dummy_header.h


# Generate the proper predefined macros for Emscripten (and not the host)
$(BUILD)/clang_predefs.h:
	$(Q)mkdir -p $(dir $@)
	$(Q)emcc $(JSFLAGS) -E -x c /dev/null -dM > $@

# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=


all: $(BUILD)/micropython.js

$(BUILD)/micropython.js: $(OBJ) library.js wrapper.js
	$(ECHO) "LINK $(BUILD)/firmware.js"
	$(Q)emcc $(LDFLAGS) -o $(BUILD)/firmware.js $(OBJ) $(JSFLAGS)
	cat wrapper.js $(BUILD)/firmware.js > $@
	cp $@ $(BUILD)/../lvgl_mp.js

min: $(BUILD)/micropython.js
	uglifyjs $< -c -o $(BUILD)/micropython.min.js

test: $(BUILD)/micropython.js $(TOP)/tests/run-tests
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../ports/javascript/node_run.sh ./run-tests

# Build clang_predefs first

Makefile: $(BUILD)/clang_predefs.h


include $(TOP)/py/mkrules.mk
