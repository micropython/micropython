SUBPACKAGE_EXAMPLE_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(SUBPACKAGE_EXAMPLE_MOD_DIR)/modexamplepackage.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(SUBPACKAGE_EXAMPLE_MOD_DIR) -DMICROPY_MODULE_BUILTIN_SUBPACKAGES=1

QSTR_DEFS += $(SUBPACKAGE_EXAMPLE_MOD_DIR)/qstrdefsexamplepackage.h
