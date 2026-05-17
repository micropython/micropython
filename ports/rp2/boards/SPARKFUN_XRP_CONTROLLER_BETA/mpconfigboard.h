// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME                   "SparkFun XRP Controller (Beta)"

// todo: We need something to check our binary size
#define MICROPY_HW_FLASH_STORAGE_BYTES          (848 * 1024)

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

// If this returns true for a pin then its irq will not be disabled on a soft reboot
int mp_hal_is_pin_reserved(int n);
#define MICROPY_HW_PIN_RESERVED(i) mp_hal_is_pin_reserved(i)

// Set the default I2C to I2C1 on pins 18 and 19 which route to the qwiic connector
#undef PICO_DEFAULT_I2C
#define PICO_DEFAULT_I2C (1)

#define MICROPY_HW_I2C1_SDA (18)
#define MICROPY_HW_I2C1_SCL (19)
