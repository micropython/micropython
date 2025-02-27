LD_FILES = boards/esp8266_2MiB.ld

MICROPY_PY_ESPNOW ?= 1
MICROPY_PY_BTREE ?= 1
MICROPY_VFS_FAT ?= 1
MICROPY_VFS_LFS2 ?= 1
MICROPY_DFU ?= 1
MICROPY_DFM ?= 1

# Add asyncio and extra micropython-lib packages (in addition to the port manifest).
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest_2MiB.py

# Configure mpconfigboard.h.
CFLAGS += -DMICROPY_ESP8266_2M
CFLAGS += -DMICROPY_DFU
CFLAGS += -DMICROPY_DFM
