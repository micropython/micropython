# User C module makefile for building into firmware

DUALMODE_MOD_DIR := $(USERMOD_DIR)

# Add our source files
SRC_USERMOD += $(DUALMODE_MOD_DIR)/dualmode_simple.c

# Add include path for mp_dualmode_v2.h
CFLAGS_USERMOD += -I$(DUALMODE_MOD_DIR)