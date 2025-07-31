// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME          "SparkFun XRP Controller"
#define MICROPY_HW_FLASH_STORAGE_BYTES (PICO_FLASH_SIZE_BYTES - 1536 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK 1
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "XRP"

// CYW43 driver configuration.
#define CYW43_USE_SPI (1)
#define CYW43_LWIP (1)
#define CYW43_GPIO (1)
#define CYW43_SPI_PIO (1)

// For debugging mbedtls - also set
// Debug level (0-4) 1=warning, 2=info, 3=debug, 4=verbose
// #define MODUSSL_MBEDTLS_DEBUG_LEVEL 1

#define MICROPY_HW_PIN_EXT_COUNT    CYW43_WL_GPIO_COUNT

int mp_hal_is_pin_reserved(int n);
#define MICROPY_HW_PIN_RESERVED(i) mp_hal_is_pin_reserved(i)

#define MICROPY_HW_USB_VID (0x1B4F)
#define MICROPY_HW_USB_PID (0x0046)

#define MICROPY_HW_I2C0_SDA  (4)
#define MICROPY_HW_I2C0_SCL  (5)

#define MICROPY_HW_I2C1_SDA  (38)
#define MICROPY_HW_I2C1_SCL  (39)

#define MICROPY_HW_SPI0_SCK  (18)
#define MICROPY_HW_SPI0_MOSI (19)
#define MICROPY_HW_SPI0_MISO (16)

#define MICROPY_HW_SPI1_SCK  (14)
#define MICROPY_HW_SPI1_MOSI (15)
#define MICROPY_HW_SPI1_MISO (12)

#define MICROPY_HW_UART0_TX (12)
#define MICROPY_HW_UART0_RX (13)
#define MICROPY_HW_UART0_CTS (14)
#define MICROPY_HW_UART0_RTS (15)

#define MICROPY_HW_UART1_TX (8)
#define MICROPY_HW_UART1_RX (9)
#define MICROPY_HW_UART1_CTS (10)
#define MICROPY_HW_UART1_RTS (11)

#define MICROPY_HW_PSRAM_CS_PIN (47)
#define MICROPY_HW_ENABLE_PSRAM (1)
