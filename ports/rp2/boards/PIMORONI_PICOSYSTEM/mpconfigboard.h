// https://shop.pimoroni.com/products/picosystem

#define MICROPY_HW_BOARD_NAME          "Pimoroni PicoSystem"
#define MICROPY_HW_FLASH_STORAGE_BYTES (7 * 1024 * 1024)

#define MICROPY_BOARD_ROOT_POINTERS uint16_t *picosystem_framebuffer;