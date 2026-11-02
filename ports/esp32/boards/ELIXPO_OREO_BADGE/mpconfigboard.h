#define MICROPY_HW_BOARD_NAME               "Oreo Badge"
#define MICROPY_HW_MCU_NAME                 "ESP32-S3"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "oreo"

#define MICROPY_HW_USB_MANUFACTURER_STRING  "Elixpo"
#define MICROPY_HW_USB_PRODUCT_FS_STRING    "Oreo Badge"

// Enable UART REPL for modules that have an external USB-UART and don't use native USB.
#define MICROPY_HW_ENABLE_UART_REPL         (1)

// I2C bus shared by the on-board MPU-6050 IMU.
#define MICROPY_HW_I2C0_SCL                 (47)
#define MICROPY_HW_I2C0_SDA                 (42)
