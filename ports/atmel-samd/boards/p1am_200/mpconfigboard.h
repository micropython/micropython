#define MICROPY_HW_BOARD_NAME "P1AM-200"
#define MICROPY_HW_MCU_NAME "samd51p20"

#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_STATUS   (&pin_PB01)

#define MICROPY_HW_NEOPIXEL (&pin_PC24)

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// QSPI CS, and QSPI SCK
#define MICROPY_PORT_B (PORT_PB10 | PORT_PB11)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PB21)
#define DEFAULT_I2C_BUS_SDA (&pin_PB20)

#define DEFAULT_SPI_BUS_SCK (&pin_PD09)
#define DEFAULT_SPI_BUS_MOSI (&pin_PD08)
#define DEFAULT_SPI_BUS_MISO (&pin_PD11)

#define DEFAULT_UART_BUS_RX (&pin_PB13)
#define DEFAULT_UART_BUS_TX (&pin_PB12)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
