// This configuration is for ESP32-C6 optimized for CSI-based motion detection.

#define MICROPY_HW_BOARD_NAME               "ESP32-C6 CSI Optimized"
#define MICROPY_HW_MCU_NAME                 "ESP32C6"

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)

// Enable CSI support (optimized for motion detection)
#define MICROPY_PY_NETWORK_WLAN_CSI         (1)
#define MICROPY_PY_NETWORK_WLAN_CSI_DEFAULT_BUFFER_SIZE (16)
