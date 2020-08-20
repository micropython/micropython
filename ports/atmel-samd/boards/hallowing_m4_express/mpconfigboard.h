#define MICROPY_HW_BOARD_NAME "Adafruit Hallowing M4 Express"
#define MICROPY_HW_MCU_NAME "samd51j19"

#define CIRCUITPY_MCU_FAMILY samd51

// Rev C

#define MICROPY_HW_LED_STATUS   (&pin_PA23)
#define MICROPY_HW_NEOPIXEL (&pin_PB16)

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// NeoPixel pin, QSPI CS, and  QSPI SCK
#define MICROPY_PORT_B (PORT_PB16 | PORT_PB10 | PORT_PB11)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define DEFAULT_I2C_BUS_SCL (&pin_PA13)
#define DEFAULT_I2C_BUS_SDA (&pin_PA12)

#define DEFAULT_SPI_BUS_SCK (&pin_PA22)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB23)
#define DEFAULT_SPI_BUS_MISO (&pin_PB22)

#define DEFAULT_UART_BUS_RX (&pin_PB13)
#define DEFAULT_UART_BUS_TX (&pin_PB12)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
