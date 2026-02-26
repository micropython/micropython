IMAI_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(IMAI_MOD_DIR)/imai_mp_iface.c $(IMAI_MOD_DIR)/model.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(IMAI_MOD_DIR)