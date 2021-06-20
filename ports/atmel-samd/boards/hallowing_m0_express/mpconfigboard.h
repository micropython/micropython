#define MICROPY_HW_BOARD_NAME "HalloWing M0 Express"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_LED_STATUS   (&pin_PA23)
#define MICROPY_HW_NEOPIXEL (&pin_PA12)

#define SPI_FLASH_MOSI_PIN          &pin_PB10
#define SPI_FLASH_MISO_PIN          &pin_PA13
#define SPI_FLASH_SCK_PIN           &pin_PB11
#define SPI_FLASH_CS_PIN            &pin_PA07

// These are pins not to reset.
// NeoPixel and for the display: Reset, Command or data, and Chip select
#define MICROPY_PORT_A        (PORT_PA01 | PORT_PA12 | PORT_PA27 | PORT_PA28)
// Data and Clock for the display
#define MICROPY_PORT_B        (PORT_PB22 | PORT_PB23)
#define MICROPY_PORT_C        (0)

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

// SWD is only available on the test pads so skip the pin objects.
#define IGNORE_PIN_PA30     1
#define IGNORE_PIN_PA31     1
