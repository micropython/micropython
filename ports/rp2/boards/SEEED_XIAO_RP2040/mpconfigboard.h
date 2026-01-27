// https://www.seeedstudio.com/XIAO-RP2040-v1-0-p-5026.html

#define MICROPY_HW_BOARD_NAME          "Seeed XIAO RP2040"
#define MICROPY_HW_FLASH_STORAGE_BYTES (1 * 1024 * 1024)

#define MICROPY_HW_SPI0_SCK  (2)
#define MICROPY_HW_SPI0_MOSI (3)
#define MICROPY_HW_SPI0_MISO (4)

#define MICROPY_HW_SPI1_SCK  (26)
#define MICROPY_HW_SPI1_MOSI (27)
#define MICROPY_HW_SPI1_MISO (28)

// NeoPixel data GPIO12, power GPIO11

// Yellow user LED GPIO18
// Blue TX LED GPIO16
// Blue RX LED GPIO17
