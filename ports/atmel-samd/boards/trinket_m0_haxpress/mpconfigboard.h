#define MICROPY_HW_BOARD_NAME "Trinket M0 Haxpress"
#define MICROPY_HW_MCU_NAME "samd21e18"

// Rev B - Black
#define MICROPY_HW_LED_STATUS   (&pin_PA10)
// #define MICROPY_HW_APA102_MOSI   (&pin_PA00)
// #define MICROPY_HW_APA102_SCK    (&pin_PA01)

#define SPI_FLASH_MOSI_PIN          &pin_PA16
#define SPI_FLASH_MISO_PIN          &pin_PA19
#define SPI_FLASH_SCK_PIN           &pin_PA17
#define SPI_FLASH_CS_PIN            &pin_PA11

// These are pins not to reset.
#define MICROPY_PORT_A        (PORT_PA00 | PORT_PA01 | PORT_PA18)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CALIBRATE_CRYSTALLESS 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA09)
#define DEFAULT_I2C_BUS_SDA (&pin_PA08)

#define DEFAULT_SPI_BUS_SCK (&pin_PA07)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA06)
#define DEFAULT_SPI_BUS_MISO (&pin_PA09)

#define DEFAULT_UART_BUS_RX (&pin_PA07)
#define DEFAULT_UART_BUS_TX (&pin_PA06)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
