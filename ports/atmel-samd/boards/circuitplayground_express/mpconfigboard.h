#define MICROPY_HW_BOARD_NAME "Adafruit CircuitPlayground Express"
#define MICROPY_HW_MCU_NAME "samd21g18"

// Don't allow touch on A0 (PA02), because it's connected to the speaker.
#define PA02_NO_TOUCH       (true)

// Salae reads 12mhz which is the limit even though we set it to the safer 8mhz.
#define SPI_FLASH_BAUDRATE  (8000000)

// On-board flash
#define SPI_FLASH_MOSI_PIN          PIN_PA20
#define SPI_FLASH_MISO_PIN          PIN_PA16
#define SPI_FLASH_SCK_PIN           PIN_PA21
#define SPI_FLASH_CS_PIN            PIN_PB22

#define SPI_FLASH_MOSI_PIN_FUNCTION PINMUX_PA20D_SERCOM3_PAD2
#define SPI_FLASH_MISO_PIN_FUNCTION PINMUX_PA16D_SERCOM3_PAD0
#define SPI_FLASH_SCK_PIN_FUNCTION  PINMUX_PA21D_SERCOM3_PAD3
#define SPI_FLASH_SERCOM            SERCOM3
#define SPI_FLASH_SERCOM_INDEX      3
#define SPI_FLASH_MOSI_PAD          2
#define SPI_FLASH_MISO_PAD          0
#define SPI_FLASH_SCK_PAD           3

// <o> Transmit Data Pinout
// <0x0=>PAD[0,1]_DO_SCK
// <0x1=>PAD[2,3]_DO_SCK
// <0x2=>PAD[3,1]_DO_SCK
// <0x3=>PAD[0,3]_DO_SCK
#define SPI_FLASH_DOPO              1
#define SPI_FLASH_DIPO              0    // same as MISO PAD

// These are pins not to reset.
// PA24 and PA25 are USB.
#define MICROPY_PORT_A        (PORT_PA16 | PORT_PA20 | PORT_PA21 | PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        (PORT_PB22)
#define MICROPY_PORT_C        (0)

#define SPEAKER_ENABLE_PIN    (&pin_PA30)

#include "external_flash/devices.h"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#include "external_flash/devices.h"

#define EXTERNAL_FLASH_DEVICE_COUNT 2
#define EXTERNAL_FLASH_DEVICES S25FL216K, \
                               GD25Q16C

#include "external_flash/external_flash.h"

#define CALIBRATE_CRYSTALLESS 1

// Explanation of how a user got into safe mode.
#define BOARD_USER_SAFE_MODE_ACTION "pressing both buttons at start up"
