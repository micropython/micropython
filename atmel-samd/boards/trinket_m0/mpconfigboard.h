#define USB_REPL

#define MICROPY_HW_BOARD_NAME "Adafruit Trinket M0 (Experimental)"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_HW_APA102_MOSI   &pin_PA01
#define MICROPY_HW_APA102_SCK    &pin_PA00

#define MICROPY_PORT_A        (PORT_PA00 | PORT_PA01 | PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        (0)

#define AUTORESET_DELAY_MS 500

#include "internal_flash.h"

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - 0x010000)
