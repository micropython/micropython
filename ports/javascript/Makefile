# customizations for lvgl
WASM_FILE_API = 1
FROZEN_DIR ?= modules
include ../../py/mkenv.mk

CROSS = 0





QSTR_DEFS = qstrdefsport.h

include $(TOP)/py/py.mk

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)


LDFLAGS ?= -m32 -Wl,--gc-sections
CFLAGS ?= -m32

#default debug options for browser debugger
JSFLAGS ?= --source-map-base http://localhost:8000

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -g4
else
CFLAGS += -Oz -g0 -DNDEBUG
endif

CFLAGS += -Wall -Werror $(INC) -std=gnu11 $(COPT)
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += $(CFLAGS_MOD)

JSFLAGS += -s USE_SDL=2 -s WASM=0
JSFLAGS += --memory-init-file 0 --js-library library.js
JSFLAGS += -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"
JSFLAGS += -s EXPORTED_FUNCTIONS="['_mp_js_init', '_mp_js_init_repl', '_mp_js_do_str', '_mp_js_process_char', '_mp_hal_get_interrupt_char', '_mp_keyboard_interrupt', '_mp_handle_pending' ]" 

ifdef EMSCRIPTEN
	# only for profiling, remove -s EMTERPRETIFY_ADVISE=1 when your EMTERPRETIFY_WHITELIST is ok
	# not using an emterpreting list *is* bad and will lead to poor performance and huge binary.
	ifdef ASYNC
		CFLAGS += -D__EMTERPRETER__=1 
		CFLAGS += -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s 'EMTERPRETIFY_FILE="micropython.binary"'
		CFLAGS += -s EMTERPRETIFY_SYNCLIST='["_mp_execute_bytecode"]' -s EMTERPRETIFY_ADVISE=1
	endif

	#check if not using emscripten-upstream branch
	ifeq (,$(findstring upstream/bin, $(EMMAKEN_COMPILER)))
		JSFLAGS += -s "BINARYEN_TRAP_MODE='clamp'"
		LDFLAGS += -Wl,-Map=$@.map,--cref
	endif  
	
	CC = emcc $(JSFLAGS)
	LD = emcc $(JSFLAGS)
	CPP = clang -E -undef -D__CPP__ -D__EMSCRIPTEN__ -U__STDC_VERSION__
	CPP += --sysroot $(EMSCRIPTEN)/system
	CPP += -include $(BUILD)/clang_predefs.h
	CPP += $(addprefix -isystem, $(shell env LC_ALL=C $(CC) $(CFLAGS_EXTRA) -E -x c++ /dev/null -v 2>&1 |sed -e '/^\#include <...>/,/^End of search/{ //!b };d'))
else
	ifdef CLANG
		CC=clang
		CPP=clang -E -D__CPP__
	else
		CC = gcc
		CPP = gcc -E -D__CPP__
	endif
endif



SRC_LIB = $(addprefix lib/,\
	utils/interrupt_char.c \
	utils/stdout_helpers.c \
	utils/pyexec.c \
	mp-readline/readline.c \
	)



LIB_SRC_C = $(addprefix lib/,\
    lv_bindings/driver/SDL/SDL_monitor.c \
    lv_bindings/driver/SDL/SDL_mouse.c \
	lv_bindings/driver/SDL/modSDL.c \
	$(LIB_SRC_C_EXTRA) \
	timeutils/timeutils.c \
	)

SRC_C = \
	main.c \
	mphalport.c \
	modutime.c \

ifdef WASM_FILE_API
	SRC_C += file.c wasm_file_api.c
endif

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

SRC_C += $(SRC_MOD)

SRC_QSTR += $(BUILD)/clang_predefs.h $(SRC_C) $(SRC_LIB) $(LIB_SRC_C)

OBJ = $(PY_O) 
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))

$(BUILD)/clang_predefs.h:
	$(Q)mkdir -p $(dir $@)
	$(Q)emcc $(CFLAGS) $(CFLAGS_EXTRA) $(JSFLAGS) -E -x c /dev/null -dM > $@

# Create `clang_predefs.h` as soon as possible, using a Makefile trick

Makefile: $(BUILD)/clang_predefs.h

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

include $(TOP)/py/mkrules.mk
