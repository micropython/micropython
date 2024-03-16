#define MICROPY_HW_BOARD_NAME "VCC-GND Studio YD-RP2040"

// Allow 1MB for the firmware image itself, allocate the remainder to the filesystem
#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - (1 * 1024 * 1024))

// https://github.com/adafruit/circuitpython/blob/main/ports/raspberrypi/boards/vcc_gnd_yd_rp2040/mpconfigboard.h#L4
#define MICROPY_HW_NEOPIXEL (&pin_GPIO23)
