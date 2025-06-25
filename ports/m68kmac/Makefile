include ../../py/mkenv.mk

CROSS_COMPILE=m68k-apple-macos-
GIT_SUBMODULES += ports/m68kmac/lib/multiversal

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

FROZEN_MANIFEST = manifest.py

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

UNAME_S := $(shell uname -s)
LD = $(CXX)
CFLAGS += $(INC) -Wall -Wdouble-promotion -Wfloat-conversion -std=c99 $(COPT)
CFLAGS += --param=min-pagesize=0
CSUPEROPT = -Os # save some code space

# Tune for Debugging or Optimization
#CFLAGS += -g  # always include debug info in the ELF
ifeq ($(DEBUG), 1)
CFLAGS += -O0
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS)) -fno-rtti -fno-exceptions

LIBS = 

SRC_C = \
	main.c \
	vfs_mac.c \
	macutil.c \
	$(BUILD)/modqd.c \

SRC_C += \
	shared/readline/readline.c \
	shared/runtime/gchelper_native.c \
	shared/runtime/interrupt_char.c \
	shared/runtime/pyexec.c \
	shared/runtime/stdout_helpers.c \
	shared/timeutils/timeutils.c \

SRC_CXX += \
	uart_core.cpp \
	retro/Console.cpp \
	retro/ConsoleWindow.cpp \
	retro/InitConsole.cpp \

SRC_S += \
	shared/runtime/gchelper_m68k.s \

SRC_QSTR += \
	vfs_mac.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \
	$(BUILD)/modqd.c \

OBJ += $(PY_CORE_O) $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

$(BUILD)/modqd.c: $(BUILD)/ tools/mkapi.py
	$(PYTHON) tools/mkapi.py lib/multiversal/defs/QuickDraw.yaml qd > $@

all: $(BUILD)/micropython.bin

$(BUILD)/micropython.code.bin: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(SIZE) $@.gdb

REZ=Rez
RINCLUDES=/Retro68-build/toolchain/RIncludes
REZFLAGS=-I$(RINCLUDES)

$(BUILD)/micropython.bin $(BUILD)/micropython.APPL $(BUILD)/micropython.dsk: $(BUILD)/micropython.code.bin
	$(REZ) $(REZFLAGS) \
		--copy "$(BUILD)/micropython.code.bin" \
		"$(RINCLUDES)/Retro68APPL.r" \
		-t "APPL" -c "mupy" \
		-o $(BUILD)/micropython.bin --cc $(BUILD)/micropython.APPL --cc $(BUILD)/micropython.dsk

.PHONY: docker-build
docker-build: $(BUILD)/modqd.py
	docker run --rm --mount type=bind,source=$(abspath $(TOP)),destination=/work ghcr.io/autc04/retro68  make -C /work/ports/m68kmac -j$(shell nproc)

.PHONY: docker-build-%
docker-build-%:
	docker run --rm --mount type=bind,source=$(abspath $(TOP)),destination=/work ghcr.io/autc04/retro68  make -C /work/ports/m68kmac $*

BASE_IMG ?= base.img
UMAC=$(HOME)/src/umac/main -w -r ~/src/umac/rom.bin
.PHONY: run
run:
	cp $(BASE_IMG) $(BUILD)/run.dsk
	hmount $(BUILD)/run.dsk
	hcopy $(BUILD)/micropython.bin ":Desktop Folder"
	$(UMAC) -d build/run.dsk

include $(TOP)/py/mkrules.mk
