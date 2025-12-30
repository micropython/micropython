// Board config for Wiznet W6100-EVB-Pico.

// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME               "W6300-EVB-Pico"
#define MICROPY_HW_FLASH_STORAGE_BYTES      (1408 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK                  (1)
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "W6300-EVB"

// Wiznet HW config.
#define MICROPY_HW_WIZNET_PIO_SPI_ID        (0)
#define MICROPY_HW_WIZNET_PIO_SPI_BAUDRATE  (20 * 1000 * 1000)
#define MICROPY_HW_WIZNET_PIO_SPI_SCK       (17)
#define MICROPY_HW_WIZNET_PIO_PIN_CS        (16)
#define MICROPY_HW_WIZNET_PIO_SPI_DATA0     (18)
#define MICROPY_HW_WIZNET_PIO_SPI_DATA1     (19)
#define MICROPY_HW_WIZNET_PIO_SPI_DATA2     (20)                // in QSPI_QUAD_MODE
#define MICROPY_HW_WIZNET_PIO_SPI_DATA3     (21)                // in QSPI_QUAD_MODE
#define MICROPY_HW_WIZNET_PIO_PIN_RST       (22)
// Connecting the INTN pin enables RECV interrupt handling of incoming data.
#define MICROPY_HW_WIZNET_PIN_INTN          (15)

// Wiznet chip type
#define _WIZCHIP_                           W6300
// QSPI Mode Configuration
// Available options:
// - QSPI_SINGLE_MODE: Uses only io0/io1 (2 data lines)
// - QSPI_DUAL_MODE:   Uses io0/io1 (2 data lines with dual mode)
// - QSPI_QUAD_MODE:   Uses io0/io1/io2/io3 (4 data lines)
#define MICROPY_HW_WIZNET_QSPI_MODE         QSPI_QUAD_MODE
