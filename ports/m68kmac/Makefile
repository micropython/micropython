ifeq ($(shell which m68k-apple-macos-gcc),)
.PHONY: default
default: docker-build
endif

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
CFLAGS += -save-temps=obj
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
	multiverse_support.c \

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

.PHONY: mkapi

DEFS = lib/multiversal/defs/MacTypes.yaml etc/MacTypesExtras.yaml
TDEFS = $(patsubst %, -t %, $(DEFS))

# $(eval $(call multiversal_module, module name, defs files))
define multiversal_module
$$(BUILD)/mod$(1).c: tools/mkapi.py $(filter-out -t,$(2)) $$(DEFS)
	$$(ECHO) "MKAPI $(1)"
	$$(Q)$$(MKDIR) -p $$(BUILD)
	$$(Q)$$(PYTHON) tools/mkapi.py -o $$@ -m $(1) $$(TDEFS) $(2)
SRC_C += $(BUILD)/mod$(1).c
SRC_QSTR += $(BUILD)/mod$(1).c
mkapi:: $$(BUILD)/mod$(1).c
endef

$(eval $(call multiversal_module,eventmgr,lib/multiversal/defs/EventMgr.yaml -t lib/multiversal/defs/QuickDraw.yaml))
$(eval $(call multiversal_module,deskmgr,lib/multiversal/defs/DeskMgr.yaml -t lib/multiversal/defs/WindowMgr.yaml -t lib/multiversal/defs/QuickDraw.yaml))
$(eval $(call multiversal_module,dialogmgr,lib/multiversal/defs/DialogMgr.yaml -t lib/multiversal/defs/WindowMgr.yaml -t lib/multiversal/defs/QuickDraw.yaml -t etc/DialogMgrExtras.yaml))
$(eval $(call multiversal_module,fontmgr,lib/multiversal/defs/FontMgr.yaml -t lib/multiversal/defs/QuickDraw.yaml))
$(eval $(call multiversal_module,mactypes,lib/multiversal/defs/MacTypes.yaml etc/MacTypesExtras.yaml))
$(eval $(call multiversal_module,menumgr,lib/multiversal/defs/MenuMgr.yaml -t lib/multiversal/defs/QuickDraw.yaml))
$(eval $(call multiversal_module,qd,lib/multiversal/defs/QuickDraw.yaml etc/QuickDrawExtras.yaml))
$(eval $(call multiversal_module,textedit,lib/multiversal/defs/TextEdit.yaml -t lib/multiversal/defs/QuickDraw.yaml))
$(eval $(call multiversal_module,toolboxevent,lib/multiversal/defs/ToolboxEvent.yaml -t lib/multiversal/defs/QuickDraw.yaml))
$(eval $(call multiversal_module,toolboxutil,lib/multiversal/defs/ToolboxUtil.yaml -t lib/multiversal/defs/EventMgr.yaml -t lib/multiversal/defs/QuickDraw.yaml))
$(eval $(call multiversal_module,windowmgr,lib/multiversal/defs/WindowMgr.yaml -t lib/multiversal/defs/QuickDraw.yaml))

OBJ += $(PY_CORE_O) $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

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
docker-build:
	./docker-run.sh make -j$(shell nproc)

BASE_IMG ?= base.img
UMAC=$(HOME)/src/umac/main -w -r ~/src/umac/rom.bin
.PHONY: run
run:
	cp $(BASE_IMG) $(BUILD)/run.dsk
	hmount $(BUILD)/run.dsk
	hcopy $(BUILD)/micropython.bin ":Desktop Folder"
	hcopy code.py ":Desktop Folder"
	hattrib -t mupy -c mupy -t text ":Desktop Folder:code.py"
	$(UMAC) -d build/run.dsk

include $(TOP)/py/mkrules.mk
