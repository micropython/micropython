#define MICROPY_HW_BOARD_NAME "Robo HAT MM1 M4"
#define MICROPY_HW_MCU_NAME "samd51g19"

#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_STATUS   (&pin_PB22)

// Salae reads 12mhz which is the limit even though we set it to the safer 8mhz.
#define SPI_FLASH_BAUDRATE  (8000000)

// On-board flash
#define SPI_FLASH_MOSI_PIN          &pin_PA12
#define SPI_FLASH_MISO_PIN          &pin_PA14
#define SPI_FLASH_SCK_PIN           &pin_PA13
#define SPI_FLASH_CS_PIN            &pin_PA15

// These are pins not to reset.
// SPI Data pins
#define MICROPY_PORT_A (0)
#define MICROPY_PORT_B (0)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)

#define DEFAULT_SPI_BUS_SCK (&pin_PB09)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB08)
#define DEFAULT_SPI_BUS_MISO (&pin_PB11)

// #define DEFAULT_UART_BUS_RX (&pin_PB03)
// #define DEFAULT_UART_BUS_TX (&pin_PB02)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
