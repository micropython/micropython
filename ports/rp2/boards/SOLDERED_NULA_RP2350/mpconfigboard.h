// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "Soldered NULA RP2350"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (14 * 1024 *1024)

// Enable networking.
#define MICROPY_PY_NETWORK 1
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "NULA_RP2350"

#define CYW43_USE_SPI (1)
#define CYW43_LWIP (1)
#define CYW43_GPIO (1)
#define CYW43_SPI_PIO (1)

// USB VID/PID
#define MICROPY_HW_USB_VID (0x2E8A)
#define MICROPY_HW_USB_PID (0x10EC)

// UART0
#define MICROPY_HW_UART0_TX  (32)
#define MICROPY_HW_UART0_RX  (33)
#define MICROPY_HW_UART0_CTS (30)
#define MICROPY_HW_UART0_RTS (31)

// I2C0
#define MICROPY_HW_I2C0_SCL  (9)
#define MICROPY_HW_I2C0_SDA  (8)

// SD Card/SPI0
#define MICROPY_HW_SPI0_SCK  (2)
#define MICROPY_HW_SPI0_MOSI (3)
#define MICROPY_HW_SPI0_MISO (0)

// SPI1
#define MICROPY_HW_SPI1_SCK  (42)
#define MICROPY_HW_SPI1_MOSI (43)
#define MICROPY_HW_SPI1_MISO (44)


// For debugging mbedtls - also set
// Debug level (0-4) 1=warning, 2=info, 3=debug, 4=verbose
// #define MODUSSL_MBEDTLS_DEBUG_LEVEL 1

#define MICROPY_HW_PIN_EXT_COUNT    CYW43_WL_GPIO_COUNT

int mp_hal_is_pin_reserved(int n);
#define MICROPY_HW_PIN_RESERVED(i) mp_hal_is_pin_reserved(i)
