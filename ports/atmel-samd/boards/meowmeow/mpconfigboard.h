#define MICROPY_HW_BOARD_NAME "Meow Meow"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_HW_LED_STATUS   (&pin_PB23)

// These are pins not to reset.
#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CALIBRATE_CRYSTALLESS 1

// Explanation of how a user got into safe mode.
#define BOARD_USER_SAFE_MODE_ACTION translate("pressing both buttons at start up.\n")

#define DEFAULT_I2C_BUS_SCL (&pin_PA01)
#define DEFAULT_I2C_BUS_SDA (&pin_PA00)

#define DEFAULT_SPI_BUS_SCK (&pin_PA15)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA14)
#define DEFAULT_SPI_BUS_MISO (&pin_PA12)

#define DEFAULT_UART_BUS_RX (&pin_PA11)
#define DEFAULT_UART_BUS_TX (&pin_PA10)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
