#define MICROPY_HW_BOARD_NAME "Waveshare RP2040-LCD-0.96"
#define MICROPY_HW_MCU_NAME "rp2040"

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO21, .sda = &pin_GPIO20}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO18, .mosi = &pin_GPIO19, .miso = &pin_GPIO16}}

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO0, .rx = &pin_GPIO1}}
