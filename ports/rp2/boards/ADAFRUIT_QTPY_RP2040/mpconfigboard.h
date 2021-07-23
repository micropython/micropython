// https://www.adafruit.com/product/4900
// https://learn.adafruit.com/adafruit-qt-py-2040/pinouts

#define MICROPY_HW_BOARD_NAME          "Adafruit QT Py RP2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (7 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x239A)
#define MICROPY_HW_USB_PID (0x80F8)

#define MICROPY_HW_UART1_TX  (20)
#define MICROPY_HW_UART1_RX  (5)
#define MICROPY_HW_UART1_CTS (10)
#define MICROPY_HW_UART1_RTS (7)

#define MICROPY_HW_I2C0_SCL  (25)
#define MICROPY_HW_I2C0_SDA  (24)

// STEMMA QT / Qwiic on I2C1
#define MICROPY_HW_I2C1_SCL  (23)
#define MICROPY_HW_I2C1_SDA  (22)

#define MICROPY_HW_SPI0_SCK  (6)
#define MICROPY_HW_SPI0_MOSI (3)
#define MICROPY_HW_SPI0_MISO (4)

// NeoPixel data GPIO12, power GPIO11

// Boot button GPIO21
