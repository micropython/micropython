#define MICROPY_HW_BOARD_NAME "Winterbloom Sol"
#define MICROPY_HW_MCU_NAME "samd51j20"

#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_STATUS (&pin_PA23)
#define MICROPY_HW_NEOPIXEL (&pin_PB03)

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// QSPI CS, QSPI SCK and NeoPixel pin
#define MICROPY_PORT_B (PORT_PB03 | PORT_PB10 | PORT_PB11)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define AUTORESET_DELAY_MS 500

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_SPI_BUS_SCK (&pin_PA17)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB23)
#define DEFAULT_SPI_BUS_MISO (&pin_PB22)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
