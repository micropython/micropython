// Board and hardware specific configuration.
#define MICROPY_HW_BOARD_NAME                   "RP2350-POE-ETH-8DI-8RO"
#define PICO_FLASH_SIZE_BYTES                   (16 * 1024 * 1024)

// UART0 is exposed on the pin header.
#define MICROPY_HW_UART0_TX                     (0)
#define MICROPY_HW_UART0_RX                     (1)
#define MICROPY_HW_UART0_CTS                    (-1)
#define MICROPY_HW_UART0_RTS                    (-1)

// UART1 is connected to the onboard isolated RS485 transceiver.
#define MICROPY_HW_UART1_TX                     (4)
#define MICROPY_HW_UART1_RX                     (5)
#define MICROPY_HW_UART1_CTS                    (-1)
#define MICROPY_HW_UART1_RTS                    (-1)

// I2C1 is connected to the onboard RTC and exposed on the pin header.
#define MICROPY_HW_I2C1_SDA                     (6)
#define MICROPY_HW_I2C1_SCL                     (7)

// SPI0 is connected to the onboard W5500.
#define MICROPY_HW_SPI0_SCK                     (34)
#define MICROPY_HW_SPI0_MOSI                    (35)
#define MICROPY_HW_SPI0_MISO                    (36)

// SPI1 is connected to the onboard SD card slot.
#define MICROPY_HW_SPI1_SCK                     (26)
#define MICROPY_HW_SPI1_MOSI                    (27)
#define MICROPY_HW_SPI1_MISO                    (28)

// Enable networking.
#define MICROPY_PY_NETWORK                      (1)
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "RP2350-8DI-8RO"

// W5500 hardware configuration. This permits network.WIZNET5K() with no args.
#define MICROPY_HW_WIZNET_SPI_ID                (0)
#define MICROPY_HW_WIZNET_SPI_BAUDRATE          (20 * 1000 * 1000)
#define MICROPY_HW_WIZNET_SPI_SCK               (34)
#define MICROPY_HW_WIZNET_SPI_MOSI              (35)
#define MICROPY_HW_WIZNET_SPI_MISO              (36)
#define MICROPY_HW_WIZNET_PIN_CS                (33)
#define MICROPY_HW_WIZNET_PIN_RST               (25)
#define MICROPY_HW_WIZNET_PIN_INTN              (32)
