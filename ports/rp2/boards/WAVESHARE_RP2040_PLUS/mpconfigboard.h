// url   :   https://www.waveshare.com/product/rp2040-plus.htm
// wiki  :   https://www.waveshare.com/wiki/RP2040-Plus

#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - (1 * 1024 * 1024))

#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x1020)
