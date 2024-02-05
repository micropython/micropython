// https://nullbits.co/bit-c-pro

#define MICROPY_HW_BOARD_NAME       "nullbits Bit-C PRO"
#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - (1 * 512 * 1024)) // 512K reserved

// RGB LED, active low
// Red LED   16
// Green LED 17
// Blue LED  18
