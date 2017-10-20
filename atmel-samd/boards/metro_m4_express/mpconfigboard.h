#define MICROPY_HW_BOARD_NAME "Metro M4 Express"
#define MICROPY_HW_MCU_NAME "samd51j19"

#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_TX   PIN_PA27
#define MICROPY_HW_LED_RX   PIN_PB06

// #define MICROPY_HW_NEOPIXEL (&pin_PB17)

#define SPI_FLASH_BAUDRATE  (1000000)

// Rev F
#define SPI_FLASH_MUX_SETTING SPI_SIGNAL_MUX_SETTING_F
#define SPI_FLASH_PAD0_PINMUX PINMUX_PB08 // MOSI
// Use default pinmux for the chip select since we manage it ourselves.
#define SPI_FLASH_PAD1_PINMUX PINMUX_PB09 // SCK
#define SPI_FLASH_PAD2_PINMUX PINMUX_DEFAULT // CS
#define SPI_FLASH_PAD3_PINMUX PINMUX_PB11 // MISO
#define SPI_FLASH_CS          PIN_PB10
#define SPI_FLASH_SERCOM      SERCOM5

#define MICROPY_PORT_A        (PORT_PA27)
#define MICROPY_PORT_B        (PORT_PB06 | PORT_PB08 | PORT_PB09 | PORT_PB11 | PORT_PB17)
#define MICROPY_PORT_C        (0)
#define MICROPY_PORT_D        (0)

#define AUTORESET_DELAY_MS 500

#include "internal_flash.h"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000)
