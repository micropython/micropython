#define MICROPY_HW_BOARD_NAME    "M5Stack NanoH2"
#define MICROPY_HW_MCU_NAME      "ESP32-H2"

// Disable WiFi/WLAN (ESP32-H2 does not have WiFi)
#define MICROPY_PY_NETWORK_WLAN             (0)
#define MICROPY_PY_ESPNOW                   (0)

#define MICROPY_HW_I2C0_SCL      (1)
#define MICROPY_HW_I2C0_SDA      (2)
