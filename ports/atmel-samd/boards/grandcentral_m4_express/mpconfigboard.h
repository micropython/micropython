#define MICROPY_HW_BOARD_NAME "Adafruit Grand Central M4 Express"
#define MICROPY_HW_MCU_NAME "samd51p20"

#define CIRCUITPY_MCU_FAMILY samd51

// This is for Rev B which is green and has the SD card slot at the edge of the board.

#define MICROPY_HW_LED_STATUS   (&pin_PB01)

#define MICROPY_HW_LED_TX   &(pin_PC30)
#define MICROPY_HW_LED_RX   &(pin_PC31)

#define MICROPY_HW_NEOPIXEL (&pin_PC24)

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// QSPI CS, and QSPI SCK
#define MICROPY_PORT_B (PORT_PB10 | PORT_PB11)
// NeoPixel pin, RX LED, TX LED
#define MICROPY_PORT_C (PORT_PC24 | PORT_PC30 | PORT_PC31)
#define MICROPY_PORT_D (0)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PB21)
#define DEFAULT_I2C_BUS_SDA (&pin_PB20)

#define DEFAULT_SPI_BUS_SCK (&pin_PD09)
#define DEFAULT_SPI_BUS_MOSI (&pin_PD08)
#define DEFAULT_SPI_BUS_MISO (&pin_PD11)

#define DEFAULT_UART_BUS_RX (&pin_PB25)
#define DEFAULT_UART_BUS_TX (&pin_PB24)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
