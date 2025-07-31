// Board and hardware specific configuration
#define MICROPY_HW_BOARD_NAME          "SparkFun Thing Plus RP2350"
#define MICROPY_HW_FLASH_STORAGE_BYTES (14 * 1024 * 1024)

// Enable networking.
#define MICROPY_PY_NETWORK 1
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "ThingPlusRP2350"

// CYW43 driver configuration.
#define CYW43_USE_SPI (1)
#define CYW43_LWIP (1)
#define CYW43_GPIO (1)
#define CYW43_SPI_PIO (1)

#define MICROPY_HW_PIN_EXT_COUNT    CYW43_WL_GPIO_COUNT

int mp_hal_is_pin_reserved(int n);
#define MICROPY_HW_PIN_RESERVED(i) mp_hal_is_pin_reserved(i)

#define MICROPY_HW_USB_VID (0x1B4F)
#define MICROPY_HW_USB_PID (0x0038)

#define MICROPY_HW_I2C0_SDA  (20)
#define MICROPY_HW_I2C0_SCL  (21)

#define MICROPY_HW_I2C1_SDA  (6)
#define MICROPY_HW_I2C1_SCL  (7)

#define MICROPY_HW_SPI0_SCK  (2)
#define MICROPY_HW_SPI0_MOSI (3)
#define MICROPY_HW_SPI0_MISO (4)

#define MICROPY_HW_SPI1_SCK  (26)
#define MICROPY_HW_SPI1_MOSI (27)
#define MICROPY_HW_SPI1_MISO (28)

#define MICROPY_HW_UART0_TX (0)
#define MICROPY_HW_UART0_RX (1)
#define MICROPY_HW_UART0_CTS (18)
#define MICROPY_HW_UART0_RTS (19)

#define MICROPY_HW_UART1_TX (4)
#define MICROPY_HW_UART1_RX (5)
#define MICROPY_HW_UART1_CTS (6)
#define MICROPY_HW_UART1_RTS (7)

#define MICROPY_HW_PSRAM_CS_PIN (8)
#define MICROPY_HW_ENABLE_PSRAM (1)
