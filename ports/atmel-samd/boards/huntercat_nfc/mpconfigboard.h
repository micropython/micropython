#define MICROPY_HW_BOARD_NAME "Electronic Cats Hunter Cat NFC"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_HW_LED_STATUS   (&pin_PA00)

#define SPI_FLASH_MOSI_PIN          &pin_PA18
#define SPI_FLASH_MISO_PIN          &pin_PA22
#define SPI_FLASH_SCK_PIN           &pin_PA19
#define SPI_FLASH_CS_PIN            &pin_PA17

#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CALIBRATE_CRYSTALLESS 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA08)
#define DEFAULT_I2C_BUS_SDA (&pin_PA09)

#define IGNORE_PIN_PA04     1
#define IGNORE_PIN_PA05     1
#define IGNORE_PIN_PA06     1
#define IGNORE_PIN_PA20     1
#define IGNORE_PIN_PA21     1
#define IGNORE_PIN_PA23     1
#define IGNORE_PIN_PA28     1
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
