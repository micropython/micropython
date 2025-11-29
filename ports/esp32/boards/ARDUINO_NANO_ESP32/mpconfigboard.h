#define MICROPY_HW_BOARD_NAME               "Arduino Nano ESP32"
#define MICROPY_HW_MCU_NAME                 "ESP32S3"

// Network config
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-nano-esp32"

#define MICROPY_HW_I2C0_SCL                 (12)
#define MICROPY_HW_I2C0_SDA                 (11)

#define MICROPY_HW_I2C1_SCL                 (8)
#define MICROPY_HW_I2C1_SDA                 (9)

#define MICROPY_HW_SPI1_MOSI                (38)
#define MICROPY_HW_SPI1_MISO                (47)
#define MICROPY_HW_SPI1_SCK                 (48)

#define MICROPY_HW_SPI2_MOSI                (10)
#define MICROPY_HW_SPI2_MISO                (17)
#define MICROPY_HW_SPI2_SCK                 (18)

#define MICROPY_HW_ENABLE_USBDEV            (1)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH    (1)
#define MICROPY_SCHEDULER_STATIC_NODES      (1)

#define MICROPY_HW_USB_VID 0x2341
#define MICROPY_HW_USB_PID 0x056B
#define MICROPY_HW_USB_MANUFACTURER_STRING "Arduino"
#define MICROPY_HW_USB_PRODUCT_FS_STRING "Nano ESP32"

#define MICROPY_BOARD_STARTUP                           NANO_ESP32_board_startup
void NANO_ESP32_board_startup(void);

#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args)     NANO_ESP32_enter_bootloader()
void NANO_ESP32_enter_bootloader(void);
