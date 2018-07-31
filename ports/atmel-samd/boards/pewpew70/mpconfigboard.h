#define MICROPY_HW_BOARD_NAME "PewPew 7.0"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_PORT_A        (PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#include "internal_flash.h"

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - 0x010000)
