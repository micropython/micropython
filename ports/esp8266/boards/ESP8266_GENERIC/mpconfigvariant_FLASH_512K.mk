LD_FILES = boards/esp8266_512kiB.ld

# Note: Use the minimal manifest.py.
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest_512kiB.py

# Configure mpconfigboard.h.
CFLAGS += -DMICROPY_ESP8266_512K
