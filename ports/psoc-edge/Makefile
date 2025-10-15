MTB_LIBS_DIR = ../../lib/mtb-psoc-edge-libs
-include $(MTB_LIBS_DIR)/mtb-bsp-setup.mk

# The only target allowed to run without BOARD defined is 'submodules' or 'help'
ifeq ($(BOARD),)
	ifneq ($(filter $(MAKECMDGOALS),submodules help),$(MAKECMDGOALS))
    	$(error No active board is set. Run "make BOARD=<target-board>" to initialize the ModusToolbox libraries and set the active board. )
	endif
endif

#TODO: Generate properly the project QSTR files
# Files that are generated and needed before the QSTR build.
# QSTR_GENERATED_HEADERS = build/pins_qstr.h
# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h #$(QSTR_GENERATED_HEADERS)
QSTR_GLOBAL_DEPENDENCIES += $(BOARD_DIR)/mpconfigboard.h #$(QSTR_GENERATED_HEADERS)


# TODO: Do we need this frozen manifest? In PSOC6 we are freezing mpip and asyncio?
# MICROPY_FROZEN_MANIFEST      ?= $(BOARD_DIR)/manifest.py
# FROZEN_MANIFEST              ?= $(MICROPY_FROZEN_MANIFEST)
#
# ifneq ($(FROZEN_MANIFEST),)
#   CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
#   CFLAGS += -DMICROPY_MODULE_FROZEN_MPY=1
#   CFLAGS += -DMICROPY_MODULE_FROZEN_STR=1
# endif

BUILD ?= build-$(BOARD)

CROSS_COMPILE ?= arm-none-eabi-

include ../../py/mkenv.mk
-include $(BOARD_DIR)/mpconfigboard.mk
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

GIT_SUBMODULES += lib/mtb-psoc-edge-libs

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(BOARD_DIR)

# TODO: Review this flags. Are they all needed?
CFLAGS_CORTEX_M33 = -mthumb -mcpu=cortex-m33 -msoft-float -fsingle-precision-constant -Wdouble-promotion -Wfloat-conversion -Wno-error=float-conversion

# TODO: These flags are copied form the mpy-psoc6 project. Do we need them all?
# std=c11 instead of std=c99 : provides "static_assert" (not available in c99) 
# -D_XOPEN_SOURCE=700 : makes sure the setenv/unsetenv headers are included
CFLAGS  += $(INC) -Wall -Werror -std=c11 $(CFLAGS_CORTEX_M33) $(COPT) -D_XOPEN_SOURCE=700
CFLAGS  += -Wno-error=double-promotion -Wno-error=overflow -Wno-error=analyzer-null-dereference -Wno-error=unused-local-typedefs -Wno-error=unused-function -Wno-error=maybe-uninitialized

# TODO: Check required additional LDFFLAGS by micropython
LD = arm-none-eabi-gcc
# LDFLAGS += -Wl,--cref -Wl,--gc-sections
# LDFLAGS += -Wl,-Map,$(BUILD)/firmware.map  -Wl,--start-group -Wl,--end-group -Wl,--print-memory-usage

#TODO: Add the pin generation. Check if the existing mpy scripts make sense for us
# or we take a similar approach as for psoc6.
#ToDo: Post adding af functionality, refactor to minimize dependent variables in py script if possible
# GEN_PINS_SRC := $(BUILD)/pins_$(BOARD).c
# HEADER_BUILD := $(BUILD)/genhdr
# GEN_PINS_HDR := $(BUILD)/genhdr/pins.h
# GEN_PINS_QSTR := $(BUILD)/pins_qstr.h
#
# GENERATED_PINS = $(GEN_PINS_SRC) $(GEN_PINS_HDR) $(GEN_PINS_QSTR)
#
# $(GENERATED_PINS): 
# 	@echo "Generating $@"
# 	$(MKDIR) -p $(BUILD)/genhdr
# 	$(PYTHON) boards/make-pins.py --gen-pin-for $(PIN_PACKAGE_FILE) --hdr $(GEN_PINS_HDR) --qstr $(GEN_PINS_QSTR) > $(GEN_PINS_SRC) 

# TODO: Check required C++ additional flags.
# Flags for optional C++ source code
# CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))
# CXXFLAGS += $(CXXFLAGS_MOD)

LDFLAGS += $(LDFLAGS_MOD)

LIBS += 

# TODO: All these shared sources are copied from
# PSOC6 files. Do we need all of them?
SHARED_SRC_C += $(addprefix shared/,\
	readline/readline.c \
	\
	runtime/gchelper_native.c \
	runtime/interrupt_char.c \
	runtime/pyexec.c \
	runtime/mpirq.c\
	runtime/stdout_helpers.c \
	runtime/sys_stdio_mphal.c \
	\
	timeutils/timeutils.c \
	)

DRIVERS_SRC_C += 

MOD_SRC_C += \
	modpsocedge.c

SRC_C = help.c \
		main.c \
		mphalport.c 

SRC_ASM +=

SRC_QSTR += $(SHARED_SRC_C) $(MOD_SRC_C)

OBJ += $(PY_O) 
OBJ += $(addprefix $(BUILD)/, $(SHARED_SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(DRIVERS_SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(MOD_SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) 
OBJ += $(addprefix $(BUILD)/, $(SRC_ASM:.s=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))

MPY_CROSS_FLAGS += -march=armv8-m.main

MPY_APP_NAME = mpy_edge_app_ns

-include $(TOP)/lib/mtb-psoc-edge-libs/mtb-makefile.mk

LDFLAGS += -Wl,-Map,$(BUILD)/$(MPY_APP_NAME).map

$(BUILD)/$(MPY_APP_NAME).elf: mtb_build_ns_info $(OBJ)
	$(info )
	$(info Linking $@ ...)
	$(Q) $(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
	$(info Linking $@ done.)
	$(Q) $(SIZE) $@ -A
	$(info )

$(BUILD)/$(MPY_APP_NAME).hex: $(BUILD)/$(MPY_APP_NAME).elf
	$(Q) $(OBJCOPY) -O ihex $^ $@

mpy_build: $(BUILD)/$(MPY_APP_NAME).hex

all: mtb_init mpy_build mtb_build_s 

rebuild: clean all

clean: mtb_clean

qdeploy: mtb_program
deploy: all mtb_program

port_help:
	@:
	$(info )
	$(info Available commands:)
	$(info ) 
	$(info   make submodules                    Initialize port required submodules.)
	$(info   make BOARD=<board_name>            Build the project for the specified board.)
	$(info   ..                                 The board name needs to be specified only the first time.)
	$(info   ..                                 Then simply run "make" to build for the same board.)
	$(info   make rebuild                       Build the project after cleaning previous build.)
	$(info   make deploy                        Build and deploy the project to the specified board.)
	$(info   make qdeploy                       Deploy the project to the specified board without rebuilding.)
	$(info   make clean                         Clean the build files.)
	$(info   make help                          Show this help.)
	$(info )

help: port_help mtb_bsp_help mtb_build_help
.DEFAULT_GOAL := all

.PHONY: all clean rebuild deploy help

include $(TOP)/py/mkrules.mk
