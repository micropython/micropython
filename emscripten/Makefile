include ../py/mkenv.mk

# clang has slightly different options to GCC
CLANG = 1
EMSCRIPTEN = 1

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk

CC = emcc -s RESERVED_FUNCTION_POINTERS=20
CPP = gcc -E
CLANG = 1
SIZE = echo
LD = emcc -s RESERVED_FUNCTION_POINTERS=20

INC += -I.
INC += -I..
INC += -I$(BUILD)

CFLAGS = $(INC) -Wall -Werror -ansi -std=gnu99 $(COPT)

#Debugging/Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0
CC += -g4
LD += -g4
else
CFLAGS += -Os -DNDEBUG
endif

CFLAGS += -D MICROPY_NLR_SETJMP=1
CFLAGS += -D MICROPY_USE_INTERNAL_PRINTF=0

LD = $(CC)
LDFLAGS = -Wl,-map,$@.map -Wl,-dead_strip -Wl,-no_pie -s EXPORTED_FUNCTIONS="['_mp_js_init', '_mp_js_run']"

LIBS =

SRC_C = \
	main.c \
	stdio_core.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

all: $(BUILD)/micropython.js

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)../tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/micropython.js: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

run:
	node repl.js

include ../py/mkrules.mk
