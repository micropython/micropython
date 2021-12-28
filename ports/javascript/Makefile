include ../../py/mkenv.mk

CROSS = 0

QSTR_DEFS = qstrdefsport.h

include $(TOP)/py/py.mk

CC = emcc
LD = emcc

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CFLAGS += -std=c99 -Wall -Werror -Wdouble-promotion -Wfloat-conversion
CFLAGS += -O3 -DNDEBUG
CFLAGS += $(INC)

SRC_SHARED = $(addprefix shared/,\
	runtime/interrupt_char.c \
	runtime/stdout_helpers.c \
	runtime/pyexec.c \
	readline/readline.c \
	)

SRC_C = \
	main.c \
	mphalport.c \
	modutime.c \

SRC_QSTR += $(SRC_C)

OBJ += $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_SHARED:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

JSFLAGS += -s ASYNCIFY
JSFLAGS += -s EXPORTED_FUNCTIONS="['_mp_js_init', '_mp_js_init_repl', '_mp_js_do_str', '_mp_js_process_char', '_mp_hal_get_interrupt_char', '_mp_sched_keyboard_interrupt']" -s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" -s --memory-init-file 0 --js-library library.js

all: $(BUILD)/micropython.js

$(BUILD)/micropython.js: $(OBJ) library.js wrapper.js
	$(ECHO) "LINK $(BUILD)/firmware.js"
	$(Q)emcc $(LDFLAGS) -o $(BUILD)/firmware.js $(OBJ) $(JSFLAGS)
	cat wrapper.js $(BUILD)/firmware.js > $@

min: $(BUILD)/micropython.js
	uglifyjs $< -c -o $(BUILD)/micropython.min.js

test: $(BUILD)/micropython.js $(TOP)/tests/run-tests.py
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../ports/javascript/node_run.sh ./run-tests.py -j1

# Disable errors for array-bounds warnings on "sp[-MP_OBJ_ITER_BUF_NSLOTS + 2]" access.
$(BUILD)/py/vm.o: CFLAGS += -Wno-error=array-bounds

include $(TOP)/py/mkrules.mk
