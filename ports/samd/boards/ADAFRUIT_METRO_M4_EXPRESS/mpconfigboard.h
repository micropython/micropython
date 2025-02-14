#define MICROPY_HW_BOARD_NAME               "Metro M4 Express Airlift"
#define MICROPY_HW_MCU_NAME                 "SAMD51J19A"

#define MICROPY_HW_XOSC32K                  (1)
#define MICROPY_HW_QSPIFLASH                GD25Q16C

#define MICROPY_HW_DEFAULT_UART_ID          (3)
#define MICROPY_HW_DEFAULT_I2C_ID           (5)
#define MICROPY_HW_DEFAULT_SPI_ID           (2)

// defines for WLAN
#define MICROPY_HW_WIFI_SPI_ID              (2)
#define MICROPY_HW_WIFI_SPI_BAUDRATE        (8000000)

#define MICROPY_HW_WIFI_SPI_SCK             (13)
#define MICROPY_HW_WIFI_SPI_MOSI            (12)
#define MICROPY_HW_WIFI_SPI_MISO            (14)

#define MICROPY_HW_NINA_ACK                 (36)
#define MICROPY_HW_NINA_GPIO1               (15)
#define MICROPY_HW_NINA_RESET               (37)
// #define MICROPY_HW_NINA_GPIO0               (33)

// defines for BLE
#define MICROPY_PY_BLUETOOTH_NINAW10        (1)

#define MICROPY_HW_BLE_UART_ID              (0)
#define MICROPY_HW_BLE_UART_BAUDRATE        (115200)

#define MICROPY_HW_BLE_UART_TX              (4)
#define MICROPY_HW_BLE_UART_RX              (7)

#define MICROPY_HW_NINA_RTS                 (12)
#define MICROPY_HW_NINA_CTS                 (36)
