ifeq ($(BOARD_VARIANT),)
LD_FILES = boards/esp8266_2MiB.ld

MICROPY_PY_ESPNOW ?= 1
MICROPY_PY_BTREE ?= 1
MICROPY_VFS_FAT ?= 1
MICROPY_VFS_LFS2 ?= 1

# Add asyncio and extra micropython-lib packages (in addition to the port manifest).
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest_2MiB.py

# Configure mpconfigboard.h.
CFLAGS += -DMICROPY_ESP8266_2M
endif

ifeq ($(BOARD_VARIANT),FLASH_1M)
LD_FILES = boards/esp8266_1MiB.ld

MICROPY_PY_ESPNOW ?= 1
MICROPY_PY_BTREE ?= 1
MICROPY_VFS_LFS2 ?= 1

# Note: Implicitly uses the port manifest.

# Configure mpconfigboard.h.
CFLAGS += -DMICROPY_ESP8266_1M
endif

ifeq ($(BOARD_VARIANT),OTA)
LD_FILES = boards/esp8266_ota.ld

MICROPY_PY_ESPNOW ?= 1
MICROPY_PY_BTREE ?= 1
MICROPY_VFS_LFS2 ?= 1

# Note: Implicitly uses the port manifest.

# Configure mpconfigboard.h.
CFLAGS += -DMICROPY_ESP8266_1M
endif

ifeq ($(BOARD_VARIANT),FLASH_512K)
LD_FILES = boards/esp8266_512kiB.ld

# Note: Use the minimal manifest.py.
FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest_512kiB.py

# Configure mpconfigboard.h.
CFLAGS += -DMICROPY_ESP8266_512K
endif
