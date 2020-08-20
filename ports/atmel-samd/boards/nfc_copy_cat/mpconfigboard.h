#define MICROPY_HW_BOARD_NAME "Electronic Cats NFC Copy Cat"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_HW_LED_STATUS   (&pin_PA14)

#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define SPI_FLASH_MOSI_PIN          &pin_PA08
#define SPI_FLASH_MISO_PIN          &pin_PA11
#define SPI_FLASH_SCK_PIN           &pin_PA09
#define SPI_FLASH_CS_PIN            &pin_PA10

// No microcontroller.nvm
#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define DEFAULT_SPI_BUS_SCK (&pin_PA17)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA16)
#define DEFAULT_SPI_BUS_MISO (&pin_PA19)

#define IGNORE_PIN_PA03     1
#define IGNORE_PIN_PA20     1
#define IGNORE_PIN_PA21     1
// USB is always used.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
#define IGNORE_PIN_PA30     1
#define IGNORE_PIN_PA31     1
#define IGNORE_PIN_PB01     1
#define IGNORE_PIN_PB02     1
#define IGNORE_PIN_PB03     1
#define IGNORE_PIN_PB04     1
#define IGNORE_PIN_PB05     1
#define IGNORE_PIN_PB06     1
#define IGNORE_PIN_PB07     1
#define IGNORE_PIN_PB08     1
#define IGNORE_PIN_PB09     1
#define IGNORE_PIN_PB10     1
#define IGNORE_PIN_PB11     1
#define IGNORE_PIN_PB12     1
#define IGNORE_PIN_PB13     1
#define IGNORE_PIN_PB14     1
#define IGNORE_PIN_PB15     1
#define IGNORE_PIN_PB16     1
#define IGNORE_PIN_PB17     1
#define IGNORE_PIN_PB22     1
#define IGNORE_PIN_PB23     1
#define IGNORE_PIN_PB30     1
#define IGNORE_PIN_PB31     1
#define IGNORE_PIN_PB00     1
