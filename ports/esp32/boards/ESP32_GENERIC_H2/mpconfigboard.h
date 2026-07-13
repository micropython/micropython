// This configuration is for a generic ESP32H2 board with 4MiB (or more) of flash.

#define MICROPY_HW_BOARD_NAME               "ESP32H2 module"
#define MICROPY_HW_MCU_NAME                 "ESP32-H2"

// Disable WiFi/WLAN (ESP32-H2 does not have WiFi)
#define MICROPY_PY_NETWORK_WLAN             (0)
#define MICROPY_PY_ESPNOW                   (0)

// Disable mDNS (doesn't build due to WiFi dependencies)
#define MICROPY_HW_ENABLE_MDNS_QUERIES      (0)
#define MICROPY_HW_ENABLE_MDNS_RESPONDER    (0)

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)
