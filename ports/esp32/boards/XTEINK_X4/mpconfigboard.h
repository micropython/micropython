#define MICROPY_HW_BOARD_NAME               "Xteink X4"
#define MICROPY_HW_MCU_NAME                 "ESP32-C3"
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-xteink-x4"

// eInk display and microSD card share this SPI bus.
#define MICROPY_HW_SPI1_SCK                 (8)
#define MICROPY_HW_SPI1_MOSI                (10)
#define MICROPY_HW_SPI1_MISO                (7)
