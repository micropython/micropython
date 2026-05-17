// Board config for Wiznet W5100S-EVB-Pico.

// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME               "W5100S-EVB-Pico"
#define MICROPY_HW_FLASH_STORAGE_BYTES      (1408 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK                  (1)
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "W5100S-EVB"

// Wiznet HW config.
#define MICROPY_HW_WIZNET_SPI_ID            (0)
#define MICROPY_HW_WIZNET_SPI_BAUDRATE      (20 * 1000 * 1000)
#define MICROPY_HW_WIZNET_SPI_SCK           (18)
#define MICROPY_HW_WIZNET_SPI_MOSI          (19)
#define MICROPY_HW_WIZNET_SPI_MISO          (16)
#define MICROPY_HW_WIZNET_PIN_CS            (17)
#define MICROPY_HW_WIZNET_PIN_RST           (20)
// Connecting the INTN pin enables RECV interrupt handling of incoming data.
#define MICROPY_HW_WIZNET_PIN_INTN          (21)
