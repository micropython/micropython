#define MICROPY_HW_BOARD_NAME "PewPew M4"
#define MICROPY_HW_MCU_NAME "samd51g19"

#define CIRCUITPY_MCU_FAMILY samd51

// These are pins not to reset.
#define MICROPY_PORT_A (0)
#define MICROPY_PORT_B (0)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define DEFAULT_I2C_BUS_SCL (&pin_PA01)
#define DEFAULT_I2C_BUS_SDA (&pin_PA00)

#define DEFAULT_SPI_BUS_SCK (&pin_PA04)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA05)
#define DEFAULT_SPI_BUS_MISO (&pin_PA00)

#define DEFAULT_UART_BUS_RX (&pin_PA05)
#define DEFAULT_UART_BUS_TX (&pin_PA00)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

// Unused pins
#define IGNORE_PIN_PA03     1
#define IGNORE_PIN_PA06     1
#define IGNORE_PIN_PA08     1
#define IGNORE_PIN_PA12     1
#define IGNORE_PIN_PA14     1
#define IGNORE_PIN_PA18     1
#define IGNORE_PIN_PA19     1
#define IGNORE_PIN_PA20     1
#define IGNORE_PIN_PA21     1

#define IGNORE_PIN_PB02     1
#define IGNORE_PIN_PB03     1
#define IGNORE_PIN_PB08     1
#define IGNORE_PIN_PB09     1
#define IGNORE_PIN_PB10     1
#define IGNORE_PIN_PB11     1

#define SAMD5x_E5x_BOD33_LEVEL (100)
