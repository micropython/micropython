#define MICROPY_HW_BOARD_NAME "Adafruit Metro M4 Airlift Lite"
#define MICROPY_HW_MCU_NAME "samd51j19"

#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_TX   (&pin_PB07)
#define MICROPY_HW_LED_RX   (&pin_PB06)

#define MICROPY_HW_LED_STATUS   (&pin_PA16)

#define MICROPY_HW_NEOPIXEL (&pin_PB22)

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// QSPI CS, QSPI SCK and NeoPixel pin
#define MICROPY_PORT_B (PORT_PB10 | PORT_PB11 | PORT_PB22)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PB03)
#define DEFAULT_I2C_BUS_SDA (&pin_PB02)

#define DEFAULT_SPI_BUS_SCK (&pin_PA13)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA12)
#define DEFAULT_SPI_BUS_MISO (&pin_PA14)

#define DEFAULT_UART_BUS_RX (&pin_PA23)
#define DEFAULT_UART_BUS_TX (&pin_PA22)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
