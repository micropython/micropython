#define MICROPY_HW_BOARD_NAME               "Arduino Nano ESP32"
#define MICROPY_HW_MCU_NAME                 "ESP32S3"

#define MICROPY_PY_MACHINE_DAC              (0)

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
#define MICROPY_HW_USB_EXTERNAL_TINYUSB     (1)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH    (1)
#define MICROPY_SCHEDULER_STATIC_NODES      (1)

#define MICROPY_HW_USB_CUSTOM_LINE_STATE_CB             NANO_ESP32_usb_callback_line_state_changed
void NANO_ESP32_usb_callback_line_state_changed(int itf, void *event);

#define MICROPY_BOARD_STARTUP                           NANO_ESP32_board_startup
void NANO_ESP32_board_startup(void);

#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args)     NANO_ESP32_enter_bootloader()
void NANO_ESP32_enter_bootloader(void);
