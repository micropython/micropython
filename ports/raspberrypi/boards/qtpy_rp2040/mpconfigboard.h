#define MICROPY_HW_BOARD_NAME "Adafruit QTPy RP2040"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO12)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO25)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO24)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO6)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO3)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO4)

#define DEFAULT_UART_BUS_RX (&pin_GPIO5)
#define DEFAULT_UART_BUS_TX (&pin_GPIO20)

// Flash chip is GD25Q64 connected over QSPI
#define TOTAL_FLASH_SIZE (8 * 1024 * 1024)
