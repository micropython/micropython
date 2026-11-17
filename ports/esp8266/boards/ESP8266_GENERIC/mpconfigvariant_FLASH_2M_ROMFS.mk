LD_FILES = boards/esp8266_2MiB_ROMFS.ld

MICROPY_PY_ESPNOW ?= 1
MICROPY_PY_BTREE ?= 1
MICROPY_VFS_FAT ?= 1
MICROPY_VFS_LFS2 ?= 1

# Add asyncio and extra micropython-lib packages (in addition to the port manifest).
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest_2MiB.py

# Configure mpconfigboard.h.
CFLAGS += -DMICROPY_ESP8266_2M -DMICROPY_VFS_ROM=1
