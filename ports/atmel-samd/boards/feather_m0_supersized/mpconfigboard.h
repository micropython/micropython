/* Adafruit Feather M0 Express with an 8MB SPI flash instead of the usual 2MB */

#define MICROPY_HW_BOARD_NAME "Hacked Feather M0 Express with 8Mbyte SPI flash"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_LED_STATUS   (&pin_PA17)

#define MICROPY_HW_NEOPIXEL (&pin_PA06)

#define SPI_FLASH_MOSI_PIN          &pin_PA08
#define SPI_FLASH_MISO_PIN          &pin_PA14
#define SPI_FLASH_SCK_PIN           &pin_PA09
#define SPI_FLASH_CS_PIN            &pin_PA13

// These are pins not to reset.
#define MICROPY_PORT_A        (PORT_PA06)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)

#define DEFAULT_SPI_BUS_SCK (&pin_PB11)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB10)
#define DEFAULT_SPI_BUS_MISO (&pin_PA12)

#define DEFAULT_UART_BUS_RX (&pin_PA11)
#define DEFAULT_UART_BUS_TX (&pin_PA10)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
