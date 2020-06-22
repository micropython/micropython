#define MICROPY_HW_BOARD_NAME "SparkFun Thing Plus - SAMD51"
#define MICROPY_HW_MCU_NAME "samd51j20"

#define CIRCUITPY_MCU_FAMILY samd51

// On-board flash
#define SPI_FLASH_MOSI_PIN &pin_PA08
#define SPI_FLASH_MISO_PIN &pin_PA11
#define SPI_FLASH_SCK_PIN  &pin_PA09
#define SPI_FLASH_CS_PIN   &pin_PA10

// These are pins not to reset.
// SPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
#define MICROPY_PORT_B (0)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)

#define DEFAULT_SPI_BUS_SCK (&pin_PB13)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB12)
#define DEFAULT_SPI_BUS_MISO (&pin_PB11)

#define DEFAULT_UART_BUS_RX (&pin_PA13)
#define DEFAULT_UART_BUS_TX (&pin_PA12)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
