// Board config for Wiznet W55RP20-EVB-Pico.

// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME               "W55RP20-EVB-Pico"
#define MICROPY_HW_FLASH_STORAGE_BYTES      (1408 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK                  (1)
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "W55RP20-EVB"

// Wiznet HW config.
#define MICROPY_HW_WIZNET_PIO_SPI_ID        (0)
#define MICROPY_HW_WIZNET_PIO_SPI_BAUDRATE  (20 * 1000 * 1000)
#define MICROPY_HW_WIZNET_PIO_SPI_SCK       (21)
#define MICROPY_HW_WIZNET_PIO_SPI_MOSI      (23)
#define MICROPY_HW_WIZNET_PIO_SPI_MISO      (22)
#define MICROPY_HW_WIZNET_PIO_PIN_CS        (20)
#define MICROPY_HW_WIZNET_PIO_PIN_RST       (24)
// Connecting the INTN pin enables RECV interrupt handling of incoming data.
#define MICROPY_HW_WIZNET_PIN_INTN          (21)
