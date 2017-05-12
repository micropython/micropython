#define USB_REPL

#define MICROPY_HW_BOARD_NAME "Adafruit Metro M0 Express"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_LED_TX   PIN_PA27
#define MICROPY_HW_LED_RX   PIN_PA31

#define MICROPY_HW_NEOPIXEL (&pin_PA30)

// Salae reads 12mhz which is the limit even though we set it to the safer 8mhz.
#define SPI_FLASH_BAUDRATE  (8000000)

#define SPI_FLASH_MUX_SETTING SPI_SIGNAL_MUX_SETTING_F
#define SPI_FLASH_PAD0_PINMUX PINMUX_UNUSED // CS
// Use default pinmux for the chip select since we manage it ourselves.
#define SPI_FLASH_PAD1_PINMUX PINMUX_PB03D_SERCOM5_PAD1 // MISO
#define SPI_FLASH_PAD2_PINMUX PINMUX_PB22D_SERCOM5_PAD2 // MOSI
#define SPI_FLASH_PAD3_PINMUX PINMUX_PB23D_SERCOM5_PAD3 // SCK
#define SPI_FLASH_CS          PIN_PA13
#define SPI_FLASH_SERCOM      SERCOM5

#define MICROPY_PORT_A        (PORT_PA13 |PORT_PA24 | PORT_PA25 | PORT_PA27 | PORT_PA30 | PORT_PA31)
#define MICROPY_PORT_B        (PORT_PB03 | PORT_PB22 | PORT_PB23)

#include "spi_flash.h"

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000)

#include "flash_S25FL216K.h"
