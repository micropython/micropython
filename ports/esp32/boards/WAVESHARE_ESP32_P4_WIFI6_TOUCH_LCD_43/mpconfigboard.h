#define MICROPY_HW_BOARD_NAME "Waveshare ESP32-P4-WIFI6-Touch-LCD-4.3"
#define MICROPY_HW_MCU_NAME "ESP32-P4"

#define MICROPY_PY_NETWORK_WLAN (1)
#define MICROPY_PY_BLUETOOTH (1)
// ESP-NOW is not supported through ESP-Hosted.
#define MICROPY_PY_ESPNOW (0)

// The USB TO UART connector uses an external bridge on UART0 (GPIO37/38).
#define MICROPY_HW_ENABLE_UART_REPL (1)

#define MICROPY_HW_I2C0_SDA (7)
#define MICROPY_HW_I2C0_SCL (8)

#define MICROPY_PY_MACHINE_SDCARD (1)
#define MICROPY_HW_SDMMC_LDO_CHAN_ID (4)
#define MICROPY_PY_MACHINE_I2S (1)
