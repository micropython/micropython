#define USB_REPL

#define MICROPY_HW_BOARD_NAME "Arduino Zero"
#define MICROPY_HW_MCU_NAME "samd21g18"

// #define MICROPY_HW_LED_MSC  PIN_PA17 // red
#define MICROPY_HW_LED_TX   PIN_PA27
#define MICROPY_HW_LED_RX   PIN_PB03

#define MICROPY_PORT_A        (PORT_PA24 | PORT_PA25 | PORT_PA27)
#define MICROPY_PORT_B        (PORT_PB03)

#include "internal_flash.h"

#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - 0x010000)
