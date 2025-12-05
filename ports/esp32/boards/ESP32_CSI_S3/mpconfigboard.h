#define MICROPY_HW_BOARD_NAME               "ESP32-S3 CSI Optimized"
#define MICROPY_HW_MCU_NAME                 "ESP32S3"

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)

#define MICROPY_HW_I2C0_SCL                 (9)
#define MICROPY_HW_I2C0_SDA                 (8)

// Enable CSI support (optimized for motion detection)
#define MICROPY_PY_NETWORK_WLAN_CSI         (1)
#define MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE (16)
