// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico 2 W"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (PICO_FLASH_SIZE_BYTES - 1024 * 1024)

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "Pico2W"

#include "enable_cyw43.h"
