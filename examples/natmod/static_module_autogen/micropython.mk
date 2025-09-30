# Makefile configuration for dual-mode module as user C module

DUALMODE_MOD_DIR := $(USERMOD_DIR)

# Add source files
SRC_USERMOD_C += $(DUALMODE_MOD_DIR)/dualmode_simple.c
SRC_USERMOD_C += $(DUALMODE_MOD_DIR)/dualmode_example.c

# Add include path for mp_dualmode.h
CFLAGS_USERMOD += -I$(DUALMODE_MOD_DIR)

# Add required QSTR definitions
# The build system will extract these from the source files