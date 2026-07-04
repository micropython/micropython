#ifndef MICROPY_HW_BOARD_NAME
// Can be set by mpconfigboard.cmake.
#define MICROPY_HW_BOARD_NAME               "Generic ESP32S31 module"
#endif
#define MICROPY_HW_MCU_NAME                 "ESP32-S31"

#define MICROPY_HW_ENABLE_USBDEV              (1)
#define MICROPY_HW_USB_CDC_DTR_RTS_BOOTLOADER (0)
#define MICROPY_HW_ESP_USB_SERIAL_JTAG        (0)
#define MICROPY_HW_ENABLE_UART_REPL           (1)
#define MICROPY_PY_MACHINE_ADC                (0)
#define MICROPY_PY_MACHINE_ADC_BLOCK          (0)

#define MICROPY_HW_I2C0_SCL                   (1)
#define MICROPY_HW_I2C0_SDA                   (0)
