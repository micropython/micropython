#define MICROPY_HW_BOARD_NAME "Adafruit Matrix Portal M4"
#define MICROPY_HW_MCU_NAME "samd51j19"

#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_STATUS   (&pin_PA14)

#define MICROPY_HW_NEOPIXEL (&pin_PA23)

#define DEFAULT_I2C_BUS_SCL (&pin_PB30)
#define DEFAULT_I2C_BUS_SDA (&pin_PB31)

#define DEFAULT_SPI_BUS_SCK (&pin_PA16)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA19)
#define DEFAULT_SPI_BUS_MISO (&pin_PA17)

#define DEFAULT_UART_BUS_RX (&pin_PA01)
#define DEFAULT_UART_BUS_TX (&pin_PA00)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
