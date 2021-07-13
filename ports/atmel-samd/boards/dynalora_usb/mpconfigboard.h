#define MICROPY_HW_BOARD_NAME "DynaLoRa_USB"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_HW_LED_STATUS   (&pin_PA27)
#define MICROPY_HW_NEOPIXEL (&pin_PA19)

#define SPI_FLASH_MOSI_PIN          &pin_PA04
#define SPI_FLASH_MISO_PIN          &pin_PA05
#define SPI_FLASH_SCK_PIN           &pin_PA07
#define SPI_FLASH_CS_PIN            &pin_PA06

// These are pins not to reset.
#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define DEFAULT_I2C_BUS_SCL (&pin_PA01)
#define DEFAULT_I2C_BUS_SDA (&pin_PA00)

#define DEFAULT_SPI_BUS_SCK (&pin_PA17)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA16)
#define DEFAULT_SPI_BUS_MISO (&pin_PA18)

#define DEFAULT_UART_BUS_RX (&pin_PA00)
#define DEFAULT_UART_BUS_TX (&pin_PA01)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA23     1
#define IGNORE_PIN_PA24     1

// Not connected
#define IGNORE_PIN_PA08     1
#define IGNORE_PIN_PA14     1
#define IGNORE_PIN_PA21     1
#define IGNORE_PIN_PA22     1
#define IGNORE_PIN_PA28     1
