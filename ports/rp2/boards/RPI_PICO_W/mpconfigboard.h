// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Raspberry Pi Pico W"

// todo: We need something to check our binary size
#define MICROPY_HW_FLASH_STORAGE_BYTES          (848 * 1024)

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "PicoW"

#include "enable_cyw43.h"
