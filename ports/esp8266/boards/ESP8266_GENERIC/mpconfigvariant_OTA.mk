LD_FILES = boards/esp8266_ota.ld

MICROPY_PY_ESPNOW ?= 1
MICROPY_PY_BTREE ?= 1
MICROPY_VFS_LFS2 ?= 1

# Note: Implicitly uses the port manifest.

# Configure mpconfigboard.h.
CFLAGS += -DMICROPY_ESP8266_1M
