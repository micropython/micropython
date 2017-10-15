#define USB_REPL

#define MICROPY_HW_BOARD_NAME "Adafruit CircuitPlayground Express"
#define MICROPY_HW_MCU_NAME "samd21g18"

// Don't allow touch on A0 (PA02), because it's connected to the speaker.
#define PA02_NO_TOUCH       (true)

// Salae reads 12mhz which is the limit even though we set it to the safer 8mhz.
#define SPI_FLASH_BAUDRATE  (8000000)

// On-board flash
#define SPI_FLASH_MUX_SETTING SPI_SIGNAL_MUX_SETTING_E
// Use default pinmux for the chip select since we manage it ourselves.
#define SPI_FLASH_PAD0_PINMUX PINMUX_PA16D_SERCOM3_PAD0 // MISO
#define SPI_FLASH_PAD1_PINMUX PINMUX_UNUSED // CS
#define SPI_FLASH_PAD2_PINMUX PINMUX_PA20D_SERCOM3_PAD2 // MOSI
#define SPI_FLASH_PAD3_PINMUX PINMUX_PA21D_SERCOM3_PAD3 // SCK

#define SPI_FLASH_CS          PIN_PB22
#define SPI_FLASH_SERCOM      SERCOM3

// PA24 and PA25 are USB.
#define MICROPY_PORT_A        (PORT_PA16 | PORT_PA20 | PORT_PA21 | PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        (PORT_PB22)

#define SPEAKER_ENABLE_PIN    (&pin_PA30)

#include "spi_flash.h"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#include "flash_S25FL216K.h"
#include "flash_GD25Q16C.h"

#define CALIBRATE_CRYSTALLESS 1

// Explanation of how a user got into safe mode.
#define BOARD_USER_SAFE_MODE "pressing both buttons at start up"
