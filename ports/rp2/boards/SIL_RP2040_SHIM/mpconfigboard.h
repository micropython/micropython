// Board config for Silicognition RP2040-Shim

#define MICROPY_HW_BOARD_NAME               "Silicognition RP2040-Shim"
#define PICO_BOOT_STAGE2_CHOOSE_GENERIC_03H 1
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES               (4 * 1024 * 1024)
#endif
#define MICROPY_HW_FLASH_STORAGE_BYTES      (3 * 1024 * 1024)

#define MICROPY_HW_USB_VID                  (0x1209)
#define MICROPY_HW_USB_PID                  (0xF502)
#undef  PICO_RP2040_B0_SUPPORTED
#define PICO_RP2040_B0_SUPPORTED            0

// Bus configuration
#define MICROPY_HW_I2C0_SCL                 (17)
#define MICROPY_HW_I2C0_SDA                 (16)

#define MICROPY_HW_SPI0_SCK                 (18)
#define MICROPY_HW_SPI0_MOSI                (19)
#define MICROPY_HW_SPI0_MISO                (20)

#define MICROPY_HW_SPI1_SCK                 (10)
#define MICROPY_HW_SPI1_MOSI                (11)
#define MICROPY_HW_SPI1_MISO                (12)

// Enable networking
#define MICROPY_PY_NETWORK                  (1)
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "RP2040-Shim"

// Wiznet HW config
#define MICROPY_HW_WIZNET_SPI_ID            (1)
#define MICROPY_HW_WIZNET_SPI_BAUDRATE      (20 * 1000 * 1000)
#define MICROPY_HW_WIZNET_SPI_SCK           (10)
#define MICROPY_HW_WIZNET_SPI_MOSI          (11)
#define MICROPY_HW_WIZNET_SPI_MISO          (12)
#define MICROPY_HW_WIZNET_PIN_CS            (21)
// Since the PoE-FeatherWing doesn't connect RST, map RST by
// default to an unused pin
#define MICROPY_HW_WIZNET_PIN_RST           (9)

// NeoPixel GPIO23, power not toggleable
// Red user LED GPIO22
