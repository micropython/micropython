#define MICROPY_HW_BOARD_NAME "WisdPi Tiny RP2040"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_LED_STATUS (&pin_GPIO25)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO5, .sda = &pin_GPIO4}}

#define MICROPY_HW_NEOPIXEL (&pin_GPIO29)
