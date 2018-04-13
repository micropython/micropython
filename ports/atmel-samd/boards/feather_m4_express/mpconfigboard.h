#define MICROPY_HW_BOARD_NAME "Feather M4 Express"
#define MICROPY_HW_MCU_NAME "samd51j19"

#define CIRCUITPY_MCU_FAMILY samd51

// This is for Rev C which is green

#define MICROPY_HW_NEOPIXEL (&pin_PB23)

// These are pins not to reset.
// QSPI Data pins and TX LED
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09)
// RX LED, QSPI CS, QSPI SCK and NeoPixel pin
#define MICROPY_PORT_B (PORT_PB10 | PORT_PB11 | PORT_PB23 )
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define AUTORESET_DELAY_MS 500

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 8192

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#include "external_flash/devices.h"

#define EXTERNAL_FLASH_DEVICE_COUNT 2
#define EXTERNAL_FLASH_DEVICES W25Q16FW, GD25Q16C

#define EXTERNAL_FLASH_QSPI_DUAL

#include "external_flash/external_flash.h"
