#define MICROPY_HW_BOARD_NAME "Adafruit Feather RP2040"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO16)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO3)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO2)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO19)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO20)

// #define DEFAULT_UART_BUS_RX (&pin_PA11)
// #define DEFAULT_UART_BUS_TX (&pin_PA10)

// Flash chip is GD25Q32 connected over QSPI
#define TOTAL_FLASH_SIZE (4 * 1024 * 1024)
