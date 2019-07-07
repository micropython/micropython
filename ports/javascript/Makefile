include ../../py/mkenv.mk

CROSS = 0

QSTR_DEFS = qstrdefsport.h

WASM_FILE_API = 1

include $(TOP)/py/py.mk

CC = emcc -g4 $(JSFLAGS)
LD = emcc -g4

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

EM_CACHE ?= $(HOME)/.emscripten_cache


BOTH_CFLAGS := -s WASM=0

CPP = clang -E

#for lvgl
CPPFLAGS += -I$(EM_CACHE)/asmjs/ports-builds/sdl2/include

ifdef WASM_FILE_API
	CPPFLAGS += -DWASM_FILE_API=1 
endif

ifdef EMSCRIPTEN
	CPP += -D__EMSCRIPTEN__ -D__CPP__
	CPP += --sysroot $(EMSCRIPTEN)/system
	CPP += -isystem $(EMSCRIPTEN)/system/include/libc
	CPP += -isystem $(EMSCRIPTEN)/system/include/libcxx
	CPP += $(CPPFLAGS)
	# Act like 'emcc'
	CPP += -U__i386 -U__i386 -Ui386 -U__SSE -U__SSE_MATH -U__SSE2 -U__SSE2_MATH -U__MMX__ -U__SSE__ -U__SSE_MATH__ -U__SSE2__ -U__SSE2_MATH__
endif

CFLAGS = -m32 -Wall $(INC) $(COPT) $(CPPFLAGS)
LDFLAGS = -m32 -Wl,-Map=$@.map,--cref -Wl,--gc-sections

OPTIM := -O2

CFLAGS += $(OPTIM) -DNDEBUG -D__EMSCRIPTEN__ -DLV_CONF_INCLUDE_SIMPLE
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += $(CFLAGS_MOD)


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
SRC_QSTR += $(SRC_C) $(LIB_SRC_C) dummy_header.h
# Append any auto-generated sources that are needed by sources listed in
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

JSFLAGS = $(BOTH_CFLAGS) -s ASSERTIONS=2 -s FORCE_FILESYSTEM=1 -s USE_SDL=2 $(OPTIM) -s EXPORTED_FUNCTIONS="['_mp_js_init', '_mp_handle_pending', '_mp_js_init_repl', '_mp_js_do_str', '_mp_js_process_char', '_mp_hal_get_interrupt_char', '_mp_keyboard_interrupt']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" --memory-init-file 0 --js-library library.js

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
