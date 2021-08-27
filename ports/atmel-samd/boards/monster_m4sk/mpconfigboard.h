#define MICROPY_HW_BOARD_NAME "Adafruit Monster M4SK"
#define MICROPY_HW_MCU_NAME "samd51j19"

#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_STATUS   (&pin_PA27)

#define DEFAULT_I2C_BUS_SCL (&pin_PA01)
#define DEFAULT_I2C_BUS_SDA (&pin_PA00)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

// Enable the use of 2 displays

#define CIRCUITPY_DISPLAY_LIMIT (2)
