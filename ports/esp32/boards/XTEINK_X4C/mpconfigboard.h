#define MICROPY_HW_BOARD_NAME               "Xteink X4 Classic"
#define MICROPY_HW_MCU_NAME                 "ESP32-S3"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-xteink-x4c"

#define MICROPY_HW_ENABLE_USBDEV            (0)
#define MICROPY_HW_ENABLE_UART_REPL         (0)

#define MICROPY_HW_I2C0_SCL                 (38)
#define MICROPY_HW_I2C0_SDA                 (39)

#define MICROPY_BOARD_STARTUP               xteink_x4c_board_startup
void xteink_x4c_board_startup(void);
