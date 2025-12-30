// Board config for Wiznet W6100-EVB-Pico2.

// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME               "W6300-EVB-Pico2"
#define MICROPY_HW_FLASH_STORAGE_BYTES      (1408 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK                  (1)
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "W6300-EVB-PICO2"

// Wiznet HW config.
#define MICROPY_HW_WIZNET_SPI_ID            (0)
#define MICROPY_HW_WIZNET_SPI_BAUDRATE      (20 * 1000 * 1000)
#define MICROPY_HW_WIZNET_SPI_SCK           (17)
#define MICROPY_HW_WIZNET_SPI_MOSI          (18)
#define MICROPY_HW_WIZNET_SPI_MISO          (19)
#define MICROPY_HW_WIZNET_PIN_CS            (16)
#define MICROPY_HW_WIZNET_PIN_RST           (22)
// Connecting the INTN pin enables RECV interrupt handling of incoming data.
#define MICROPY_HW_WIZNET_PIN_INTN          (15)
