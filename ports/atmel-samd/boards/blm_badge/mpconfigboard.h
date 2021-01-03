#define MICROPY_HW_BOARD_NAME "Adafruit BLM Badge"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_HW_LED_STATUS   (&pin_PA03)

#define MICROPY_PORT_A        (PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define DEFAULT_I2C_BUS_SCL (&pin_PA01)
#define DEFAULT_I2C_BUS_SDA (&pin_PA00)

#define DEFAULT_UART_BUS_RX (&pin_PA01)
#define DEFAULT_UART_BUS_TX (&pin_PA00)

#define IGNORE_PIN_PA09     1
#define IGNORE_PIN_PA12     1
#define IGNORE_PIN_PA13     1
#define IGNORE_PIN_PA14     1
#define IGNORE_PIN_PA15     1
#define IGNORE_PIN_PA16     1
#define IGNORE_PIN_PA17     1
#define IGNORE_PIN_PA18     1
#define IGNORE_PIN_PA19     1
#define IGNORE_PIN_PA20     1
#define IGNORE_PIN_PA21     1
#define IGNORE_PIN_PA22     1
#define IGNORE_PIN_PA23     1
// USB is always used.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
#define IGNORE_PIN_PA27     1
#define IGNORE_PIN_PA28     1
#define IGNORE_PIN_PA30     1
#define IGNORE_PIN_PA31     1
