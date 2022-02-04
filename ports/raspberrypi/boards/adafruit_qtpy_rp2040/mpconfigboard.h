#define MICROPY_HW_BOARD_NAME "Adafruit QT Py RP2040"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO12)
#define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO11)

#define CIRCUITPY_BOARD_I2C         (2)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO25, .sda = &pin_GPIO24}, \
                                     {.scl = &pin_GPIO23, .sda = &pin_GPIO22}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO6, .mosi = &pin_GPIO3, .miso = &pin_GPIO4}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO20, .rx = &pin_GPIO5}}
