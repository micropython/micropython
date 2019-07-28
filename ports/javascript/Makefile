include ../../py/mkenv.mk

CROSS = 0

QSTR_DEFS = qstrdefsport.h

include $(TOP)/py/py.mk

CC = emcc -g4
LD = emcc -g4

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

ifdef EMSCRIPTEN
	# only for profiling, remove -s EMTERPRETIFY_ADVISE=1 when your EMTERPRETIFY_WHITELIST is ok
	# not using an emterpreting list *is* bad and will lead to poor performance and huge binary.
	ifdef ASYNC
		CFLAGS += -D__EMTERPRETER__=1 
		CFLAGS += -s EMTERPRETIFY=1 -s EMTERPRETIFY_ASYNC=1 -s 'EMTERPRETIFY_FILE="micropython.binary"'
		CFLAGS += -s EMTERPRETIFY_SYNCLIST='["_mp_execute_bytecode"]' -s EMTERPRETIFY_ADVISE=1
	endif

	CC = emcc
	CPP = clang -E -undef -D__CPP__ -D__EMSCRIPTEN__
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

CFLAGS = -m32 -Wall -Werror $(INC) -std=c99 $(COPT)
LDFLAGS = -m32 -Wl,-Map=$@.map,--cref -Wl,--gc-sections

CFLAGS += -O0 -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections

SRC_LIB = $(addprefix lib/,\
	utils/interrupt_char.c \
	utils/stdout_helpers.c \
	utils/pyexec.c \
	mp-readline/readline.c \
	)

SRC_C = \
	main.c \
	mphalport.c \
	modutime.c \

#force preprocessor env to be created before qstr extraction
ifdef EMSCRIPTEN
	SRC_QSTR += $(BUILD)/clang_predefs.h
endif

SRC_QSTR += $(SRC_C)



OBJ = 
OBJ = $(PY_O) 
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

JSFLAGS = -O0  -s "BINARYEN_TRAP_MODE='clamp'" --memory-init-file 0 --js-library library.js
JSFLAGS += -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']"
JSFLAGS += -s EXPORTED_FUNCTIONS="['_mp_js_init', '_mp_js_init_repl', '_mp_js_do_str', '_mp_js_process_char', '_mp_hal_get_interrupt_char', '_mp_keyboard_interrupt']" 

$(BUILD)/clang_predefs.h:
        $(Q)mkdir -p $@
	$(Q)emcc $(CFLAGS) $(CFLAGS_EXTRA) $(JSFLAGS) -E -x c /dev/null -dM > $@

# Create `clang_predefs.h` as soon as possible, using a Makefile trick

Makefile: $(BUILD)/clang_predefs.h

all: $(BUILD)/micropython.js

$(BUILD)/micropython.js: $(OBJ) library.js wrapper.js
	$(ECHO) "LINK $(BUILD)/firmware.js"
	$(Q)emcc $(LDFLAGS) -o $(BUILD)/firmware.js $(OBJ) $(JSFLAGS)
	cat wrapper.js $(BUILD)/firmware.js > $@

min: $(BUILD)/micropython.js
	uglifyjs $< -c -o $(BUILD)/micropython.min.js

test: $(BUILD)/micropython.js $(TOP)/tests/run-tests
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../ports/javascript/node_run.sh ./run-tests

include $(TOP)/py/mkrules.mk
