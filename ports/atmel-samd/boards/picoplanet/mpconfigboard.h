#define MICROPY_HW_BOARD_NAME "PicoPlanet"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_PORT_A        (PORT_PA00 | PORT_PA01)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define IGNORE_PIN_PA00     1
#define IGNORE_PIN_PA01     1
#define IGNORE_PIN_PA10     1
#define IGNORE_PIN_PA11     1
#define IGNORE_PIN_PA12     1
#define IGNORE_PIN_PA13     1
#define IGNORE_PIN_PA14     1
#define IGNORE_PIN_PA15     1
#define IGNORE_PIN_PA18     1
#define IGNORE_PIN_PA19     1
#define IGNORE_PIN_PA20     1
#define IGNORE_PIN_PA21     1
#define IGNORE_PIN_PA22     1
#define IGNORE_PIN_PA23     1
// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
#define IGNORE_PIN_PA27     1
#define IGNORE_PIN_PA28     1
#define IGNORE_PIN_PA31     1

#define DEFAULT_I2C_BUS_SCL (&pin_PA09)
#define DEFAULT_I2C_BUS_SDA (&pin_PA08)

#define DEFAULT_SPI_BUS_MISO (&pin_PA30)
#define DEFAULT_SPI_BUS_SCK (&pin_PA17)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA16)

// #define CIRCUITPY_RGB_STATUS_R (&pin_PA06)
// #define CIRCUITPY_RGB_STATUS_G (&pin_PA05)
// #define CIRCUITPY_RGB_STATUS_B (&pin_PA07)
// #define CIRCUITPY_RGB_STATUS_INVERTED_PWM

#define MICROPY_HW_LED_STATUS   (&pin_PA06)
