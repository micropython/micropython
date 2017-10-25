#define USB_REPL

#define MICROPY_HW_BOARD_NAME "Trinket M0 Haxpress"
#define MICROPY_HW_MCU_NAME "samd21e18"

// Rev B - Black
#define MICROPY_HW_APA102_MOSI   (&pin_PA00)
#define MICROPY_HW_APA102_SCK    (&pin_PA01)

// Salae reads 12mhz which is the limit even though we set it to the
// safer 8mhz.
#define SPI_FLASH_BAUDRATE (8000000)

#define SPI_FLASH_MUX_SETTING SPI_SIGNAL_MUX_SETTING_D
#define SPI_FLASH_PAD0_PINMUX PINMUX_PA16D_SERCOM3_PAD0 // MOSI
#define SPI_FLASH_PAD1_PINMUX PINMUX_PA17D_SERCOM3_PAD1 // SCK
#define SPI_FLASH_PAD2_PINMUX PINMUX_UNUSED // Use default pinmux for the chip
                                            // select since we manage it
                                            // ourselves.
#define SPI_FLASH_PAD3_PINMUX PINMUX_PA19D_SERCOM3_PAD3 // MISO
#define SPI_FLASH_SERCOM SERCOM3

#define SPI_FLASH_CS PIN_PA11

#define MICROPY_PORT_A        (PORT_PA00 | PORT_PA01 | PORT_PA11 | PORT_PA16 |\
                               PORT_PA17 | PORT_PA18 | PORT_PA19 | PORT_PA24 |\
                               PORT_PA25)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CALIBRATE_CRYSTALLESS 1

#include "spi_flash.h"

#define CIRCUITPY_INTERNAL_NVM_SIZE 256
#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#include "flash_W25Q32BV.h"

