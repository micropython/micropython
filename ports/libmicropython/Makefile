include ../../py/mkenv.mk
-include mpconfigport.mk

all: lib

# QSTR definitions (must come before including py.mk).
QSTR_DEFS = $(MICROPY_QSTRDEFSEMBED_H)
QSTR_GLOBAL_DEPENDENCIES = $(MICROPY_MPCONFIGEMBED_H)

# Include py core make definitions.
include $(TOP)/py/py.mk

# Force a full QSTR rebuild if mpconfigembed.h changes.
$(HEADER_BUILD)/qstrdefs.generated.h: $(MICROPY_MPCONFIGEMBED_H)

INC += -I.
INC += -I..
INC += -I$(TOP)
INC += -I$(BUILD)

# Compiler settings.
CWARN = -Wall -Werror
CWARN += -Wpointer-arith -Wuninitialized
CFLAGS = $(INC) $(CWARN) -std=gnu99 -DUNIX $(CFLAGS_MOD) $(COPT) $(CFLAGS_EXTRA)

# Debugging/Optimization
ifdef DEBUG
COPT = -O0
else
COPT = -Os -DNDEBUG
# See explanation in ports/unix/Makefile.
CFLAGS += -U _FORTIFY_SOURCE
endif

CFLAGS += -g

LDFLAGS_ARCH = -Wl,-map,$@.map

ifeq ($(CC),clang)
# Use gcc syntax for map file.
LDFLAGS_ARCH += -Wl,-Map=$@.map,--cref
endif

LDFLAGS = $(LDFLAGS_MOD) $(LDFLAGS_ARCH) -lm $(LDFLAGS_EXTRA)

SRC_C += \
	libmicropython.c \
	lib/utils/interrupt_char.c \
	lib/utils/gchelper_generic.c \
	lib/utils/stdout_helpers.c \

OBJ = $(PY_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(LIB_SRC_C:.c=.o))

# List of sources for qstr extraction.
SRC_QSTR += $(SRC_C) $(LIB_SRC_C)
# Append any auto-generated sources that are needed by sources listed in.
# SRC_QSTR
SRC_QSTR_AUTO_DEPS +=

include $(TOP)/py/mkrules.mk

CLEAN_EXTRA += $(LIBMICROPYTHON)

# Allow embedder to specify an mpconfigembed.h.
ifneq ($(MICROPY_MPCONFIGEMBED_H),)
CFLAGS += -DMICROPY_MPCONFIGEMBED_H='<$(MICROPY_MPCONFIGEMBED_H)>'
endif
