#define USB_REPL

#define MICROPY_HW_BOARD_NAME "Adafruit Feather M0 Express"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_NEOPIXEL (&pin_PA06)

// Salae reads 12mhz which is the limit even though we set it to the safer 8mhz.
#define SPI_FLASH_BAUDRATE  (8000000)

#define SPI_FLASH_MUX_SETTING SPI_SIGNAL_MUX_SETTING_C
#define SPI_FLASH_PAD0_PINMUX PINMUX_PA08D_SERCOM2_PAD0 // MOSI
// Use default pinmux for the chip select since we manage it ourselves.
#define SPI_FLASH_PAD1_PINMUX PINMUX_PA09D_SERCOM2_PAD1 // SCK
#define SPI_FLASH_PAD2_PINMUX PINMUX_PA14C_SERCOM2_PAD2 // MISO
#define SPI_FLASH_PAD3_PINMUX PINMUX_UNUSED // SCK
#define SPI_FLASH_SERCOM      SERCOM2

#define SPI_FLASH_CS          PIN_PA13

#define MICROPY_PORT_A        (PORT_PA06 | PORT_PA08 | PORT_PA09 | PORT_PA14 | PORT_PA13 | PORT_PA14 | PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        ( 0 )

#include "spi_flash.h"

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000)

#include "flash_S25FL216K.h"
