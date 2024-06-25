// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Seeed Xiao ESP32-C6 4MB Flash 512KB SRAM"
#define MICROPY_HW_MCU_NAME         "ESP32-C6FH4"

#define MICROPY_HW_LED_STATUS (&pin_GPIO15)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO23, .sda = &pin_GPIO22}}

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO19, .mosi = &pin_GPIO18, .miso = &pin_GPIO20}}

// TXD0 and RXD0
#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO16, .rx = &pin_GPIO17}}

// For entering safe mode, use BOOT button
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO9)
