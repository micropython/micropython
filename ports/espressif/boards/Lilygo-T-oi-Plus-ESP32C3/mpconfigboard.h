// Define board
#define MICROPY_HW_BOARD_NAME       "LILYGO TTGO T-OI PLUS"
#define MICROPY_HW_MCU_NAME         "ESP32-C3"

#define MICROPY_HW_LED_STATUS       (&pin_GPIO3)

// I2C
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO18, .sda = &pin_GPIO19}}

// UART
#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO21, .rx = &pin_GPIO20}}
