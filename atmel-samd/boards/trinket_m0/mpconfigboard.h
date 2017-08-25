#define USB_REPL

#define MICROPY_HW_BOARD_NAME "Adafruit Trinket M0"
#define MICROPY_HW_MCU_NAME "samd21e18"

// Rev B - Black
#define MICROPY_HW_APA102_MOSI   (&pin_PA00)
#define MICROPY_HW_APA102_SCK    (&pin_PA01)

#define MICROPY_PORT_A        (PORT_PA00 | PORT_PA01 | PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        (0)

#include "internal_flash.h"

#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - 0x010000)
