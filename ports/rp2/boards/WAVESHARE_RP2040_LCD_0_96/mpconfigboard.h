//url   :   https://www.waveshare.com/product/rp2040-lcd-0.96.htm
//wiki  :   https://www.waveshare.com/wiki/RP2040-LCD-0.96
// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Waveshar RP2040-LCD-0.96"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (1408 * 1024)

#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x1021)


#define MICROPY_HW_SPI1_SCK  (10u)
#define MICROPY_HW_SPI1_MOSI (11u)
#define MICROPY_HW_SPI1_MISO (8u)

// LCD backlight control pin GPIO25
