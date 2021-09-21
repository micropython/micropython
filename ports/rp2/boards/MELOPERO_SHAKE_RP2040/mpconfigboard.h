// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Melopero Shake RP2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (15 * 1024 * 1024)

#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x1005)

// Qwiic on I2C1
#define MICROPY_HW_I2C1_SCL  (3)
#define MICROPY_HW_I2C1_SDA  (2)

#define MICROPY_HW_SPI0_SCK  (18)
#define MICROPY_HW_SPI0_MOSI (19)
#define MICROPY_HW_SPI0_MISO (20)

