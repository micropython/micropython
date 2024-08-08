#define MICROPY_HW_BOARD_NAME "Raspberry Pi Pico 2"
#define MICROPY_HW_MCU_NAME "rp2350a"

#define MICROPY_HW_LED_STATUS (&pin_GPIO25)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO5, .sda = &pin_GPIO4}}
