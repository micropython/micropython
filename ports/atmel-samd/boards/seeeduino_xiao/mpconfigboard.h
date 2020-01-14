#define MICROPY_HW_BOARD_NAME "Seeeduino XIAO"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define DEFAULT_I2C_BUS_SCL (&pin_PA09) // was PA23
#define DEFAULT_I2C_BUS_SDA (&pin_PA08) // was PA22

#define DEFAULT_SPI_BUS_SCK (&pin_PA07) // was PA17
#define DEFAULT_SPI_BUS_MOSI (&pin_PA06) // was PA16
#define DEFAULT_SPI_BUS_MISO (&pin_PA05)  // was PA19

#define DEFAULT_UART_BUS_RX (&pin_PB09) // was PA11
#define DEFAULT_UART_BUS_TX (&pin_PB08) // was PA10

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
