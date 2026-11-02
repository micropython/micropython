#define MICROPY_HW_BOARD_NAME "SparkFun IoT RedBoard ESP32-P4"
#define MICROPY_HW_MCU_NAME "ESP32P4"

#define MICROPY_PY_ESPNOW                (0)

#define MICROPY_PY_MACHINE_SDCARD           (1)
#define MICROPY_HW_SDMMC_LDO_CHAN_ID        (4)
// This board wires the SD/MMC card to SDMMC slot 0 with a full 4-bit bus.
#define MICROPY_HW_SDMMC_DEFAULT_SLOT       (0)
#define MICROPY_HW_SDMMC_DEFAULT_WIDTH      (4)

#ifndef USB_SERIAL_JTAG_PACKET_SZ_BYTES
#define USB_SERIAL_JTAG_PACKET_SZ_BYTES (64)
#endif

// Native USB is used for the REPL, so disable the UART REPL.
#define MICROPY_HW_ENABLE_UART_REPL     (0)

#define MICROPY_PY_MACHINE_I2S          (1)

// Enable Wi-Fi and Bluetooth.
#define MICROPY_PY_NETWORK_WLAN         (1)
#define MICROPY_PY_BLUETOOTH            (1)

#define MICROPY_HW_UART0_TX             (37)
#define MICROPY_HW_UART0_RX             (38)

#define MICROPY_HW_I2C0_SDA             (26)
#define MICROPY_HW_I2C0_SCL             (27)

#define MICROPY_HW_SPI1_SCK             (9)
#define MICROPY_HW_SPI1_MOSI            (8)
#define MICROPY_HW_SPI1_MISO            (10)
