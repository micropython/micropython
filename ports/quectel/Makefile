TOP := ../..
ifneq ($(HeliosSDK_PATH),)
ROOT := $(HeliosSDK_PATH)
else
ROOT := $(TOP)/lib/helios
endif

include ../../py/mkenv.mk
include quectel.mk

CROSS = 1

ifeq ($(CROSS), 1)
CFLAGS = $(INC) $(QUEC_MOD_CFLAGS) $(PLAT_CFLAGS) $(PLAT_DFLAGS) $(COPT)
else
LD = gcc
CFLAGS = $(INC) -Wall -Werror -Wdouble-promotion -Wfloat-conversion -std=c99 $(COPT)
LDFLAGS = -Wl,-Map=$@.map,--cref -Wl,--gc-sections
endif

CFLAGS += -mcpu=cortex-a5 -mtune=generic-armv7-a -mthumb \
		  -mfpu=neon-vfpv4 -mfloat-abi=hard -mno-unaligned-access

CSUPEROPT = -Os # save some code space

GIT_SUBMODULES += lib/helios

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# MicroPython feature configurations
MICROPY_ROM_TEXT_COMPRESSION ?= 1

FROZEN_MANIFEST = $(QUEC_PY_MOD)

ifeq ($(OS),Windows_NT)
export MICROPY_MPYCROSS = $(TOP)/mpy-cross/build/mpy-cross.exe
else	
export MICROPY_MPYCROSS = $(TOP)/mpy-cross/build/mpy-cross
endif

# include py core make definitions
include $(TOP)/py/py.mk
include $(TOP)/extmod/extmod.mk

ifeq ($(CROSS), 1)
CROSS_COMPILE ?= arm-none-eabi-
endif

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += $(MICROPYTHON_CFLAGS_INC)
INC += $(QUEC_INC)

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))
CXXFLAGS += $(CXXFLAGS_MOD)

# Flags for user C modules
CFLAGS += $(CFLAGS_MOD)
LDFLAGS += $(LDFLAGS_MOD)

LIBS = lib/littlefs/lfs2.c \
	lib/littlefs/lfs2_util.c

SHARED_SRC_C = shared/libc/printf.c \
	shared/readline/readline.c \
	shared/runtime/pyexec.c \
	shared/runtime/sys_stdio_mphal.c \
	shared/runtime/stdout_helpers.c \
	shared/netutils/netutils.c \
	shared/runtime/interrupt_char.c \
	shared/timeutils/timeutils.c

SRC_MOD += $(QUEC_SRC_MOD)

SRC_C += $(SRC_MOD) \
		 $(SHARED_SRC_C) \
		 $(SRC_EXTMOD_C) \
		 $(LIBS) \
		 $(QUEC_SRC) \
		 $(BUILD)/frozen_content.c

SRC_CXX += $(SRC_MOD_CXX)

SRC_QSTR += $(SRC_MOD) $(SRC_MOD_CXX) $(SHARED_SRC_C) $(SRC_EXTMOD_C)

OBJ += $(PY_CORE_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))

ifeq ($(CROSS), 1)
all: $(BUILD)/firmware.a
else
all: $(BUILD)/firmware.elf
endif

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(PYTHON) $(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.a: $(OBJ)
	$(ECHO) "AR $@"
	$(Q)$(AR) -cr $@ $^
	cd $(ROOT) && helios make ../../../micropython $(BOARD)

include $(TOP)/py/mkrules.mk
