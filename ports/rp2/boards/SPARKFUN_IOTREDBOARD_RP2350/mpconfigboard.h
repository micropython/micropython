// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "SparkFun IoT RedBoard RP2350"
#define MICROPY_HW_FLASH_STORAGE_BYTES          (PICO_FLASH_SIZE_BYTES - 1536 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK 1
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "IoTRedBoardRP2350"

#define CYW43_USE_SPI (1)
#define CYW43_LWIP (1)
#define CYW43_GPIO (1)
#define CYW43_SPI_PIO (1)

// USB VID/PID
#define MICROPY_HW_USB_VID (0x1B4F)
#define MICROPY_HW_USB_PID (0x0047)

// UART0
#define MICROPY_HW_UART0_TX  (0)
#define MICROPY_HW_UART0_RX  (1)
#define MICROPY_HW_UART0_CTS (30)
#define MICROPY_HW_UART0_RTS (31)

// UART1
#define MICROPY_HW_UART1_TX  (40)
#define MICROPY_HW_UART1_RX  (41)
#define MICROPY_HW_UART1_CTS (42)
#define MICROPY_HW_UART1_RTS (43)

// I2C0
#define MICROPY_HW_I2C0_SCL  (5)
#define MICROPY_HW_I2C0_SDA  (4)

// I2C1
#define MICROPY_HW_I2C1_SCL  (31)
#define MICROPY_HW_I2C1_SDA  (30)

// SPI0
#define MICROPY_HW_SPI0_SCK  (22)
#define MICROPY_HW_SPI0_MOSI (23)
#define MICROPY_HW_SPI0_MISO (20)

// SD Card/SPI1
#define MICROPY_HW_SPI1_SCK  (10)
#define MICROPY_HW_SPI1_MOSI (11)
#define MICROPY_HW_SPI1_MISO (8)

// PSRAM
#define MICROPY_HW_PSRAM_CS_PIN (47)
#define MICROPY_HW_ENABLE_PSRAM (1)

// #include "enable_cyw43.h"

// For debugging mbedtls - also set
// Debug level (0-4) 1=warning, 2=info, 3=debug, 4=verbose
// #define MODUSSL_MBEDTLS_DEBUG_LEVEL 1

#define MICROPY_HW_PIN_EXT_COUNT    CYW43_WL_GPIO_COUNT

int mp_hal_is_pin_reserved(int n);
#define MICROPY_HW_PIN_RESERVED(i) mp_hal_is_pin_reserved(i)
