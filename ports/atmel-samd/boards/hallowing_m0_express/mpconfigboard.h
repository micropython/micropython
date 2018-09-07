#define MICROPY_HW_BOARD_NAME "HalloWing M0 Express"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_NEOPIXEL (&pin_PA12)

// Clock rates are off: Saleae reads 12MHz which is the limit even though we set it to the safer 8MHz.
#define SPI_FLASH_BAUDRATE  (8000000)

#define SPI_FLASH_MOSI_PIN          PIN_PB10
#define SPI_FLASH_MISO_PIN          PIN_PA13
#define SPI_FLASH_SCK_PIN           PIN_PB11
#define SPI_FLASH_CS_PIN            PIN_PA07
#define SPI_FLASH_MOSI_PIN_FUNCTION PINMUX_PB10D_SERCOM4_PAD2
#define SPI_FLASH_MISO_PIN_FUNCTION PINMUX_PA13D_SERCOM4_PAD1
#define SPI_FLASH_SCK_PIN_FUNCTION  PINMUX_PB11D_SERCOM4_PAD3
#define SPI_FLASH_SERCOM            SERCOM4
#define SPI_FLASH_SERCOM_INDEX      4
#define SPI_FLASH_MOSI_PAD          2
#define SPI_FLASH_MISO_PAD          1
#define SPI_FLASH_SCK_PAD           3
// <o> Transmit Data Pinout
// <0x0=>PAD[0,1]_DO_SCK
// <0x1=>PAD[2,3]_DO_SCK
// <0x2=>PAD[3,1]_DO_SCK
// <0x3=>PAD[0,3]_DO_SCK
#define SPI_FLASH_DOPO              0x1
#define SPI_FLASH_DIPO              1   // same as MISO pad

// These are pins not to reset.
#define MICROPY_PORT_A        ( PORT_PA01 | PORT_PA07 | PORT_PA12 | PORT_PA13 | PORT_PA24 | PORT_PA25 | PORT_PA27 | PORT_PA28)
#define MICROPY_PORT_B        ( PORT_PB10 | PORT_PB11 | PORT_PB22 | PORT_PB23 )
#define MICROPY_PORT_C        ( 0 )

#include "external_flash/external_flash.h"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#include "external_flash/devices.h"

#define EXTERNAL_FLASH_DEVICE_COUNT 2

#define EXTERNAL_FLASH_DEVICES W25Q64JV_IQ, \
                               GD25Q64C

#include "external_flash/external_flash.h"

#define DEFAULT_I2C_BUS_SCL (&pin_PA17)
#define DEFAULT_I2C_BUS_SDA (&pin_PA16)

#define DEFAULT_SPI_BUS_SCK (&pin_PB23)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB22)
#define DEFAULT_SPI_BUS_MISO (&pin_PB03)

#define DEFAULT_UART_BUS_RX (&pin_PA09)
#define DEFAULT_UART_BUS_TX (&pin_PA10)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

#define CIRCUITPY_DISPLAYIO
