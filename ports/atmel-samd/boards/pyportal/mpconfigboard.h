#define MICROPY_HW_BOARD_NAME "Adafruit PyPortal"
#define MICROPY_HW_MCU_NAME "samd51j20"

#define CIRCUITPY_MCU_FAMILY samd51

// This is for Rev B

#define MICROPY_HW_LED_STATUS   (&pin_PA27)

#define MICROPY_HW_NEOPIXEL (&pin_PB22)

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// QSPI CS, and QSPI SCK
#define MICROPY_PORT_B (PORT_PB10 | PORT_PB11 | PORT_PB22)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define DEFAULT_I2C_BUS_SCL (&pin_PB03)
#define DEFAULT_I2C_BUS_SDA (&pin_PB02)

#define DEFAULT_SPI_BUS_SCK (&pin_PA13)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA12)
#define DEFAULT_SPI_BUS_MISO (&pin_PA14)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
