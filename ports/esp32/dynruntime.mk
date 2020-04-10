# Makefile to include after py/dynruntime.mk in order to add esp32-specific rules
# for building native modules that include esp-idf header files and call esp-idf functions.

PORT_DIR := $(MPY_DIR)/ports/esp32

MPY_DYNRUNTIME := 1
include $(PORT_DIR)/Makefile

CFLAGS += -std=gnu99 # override -std=c99 in dynruntime.mk
CFLAGS += $(INC) $(INC_ESPCOMP) $(CFLAGS_MOD)

# Make all module object files depend on sdkconfig
$(SRC_O): $(SDKCONFIG_H)

# Tell mpy_ld to load port-specific function table
PORT_FUN=--portfun $(PORT_DIR)/port_fun.inc
