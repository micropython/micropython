#define MICROPY_HW_BOARD_NAME "CircuitBrains Basic"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define CIRCUITPY_MCU_FAMILY samd21

#define MICROPY_HW_LED_STATUS   (&pin_PA14)

// On-board flash
#define SPI_FLASH_MOSI_PIN          &pin_PA16
#define SPI_FLASH_MISO_PIN          &pin_PA18
#define SPI_FLASH_SCK_PIN           &pin_PA17
#define SPI_FLASH_CS_PIN            &pin_PA19

// These are pins not to reset.
#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define SPI_FLASH_BAUDRATE  (8000000)

#define CALIBRATE_CRYSTALLESS 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA05)
#define DEFAULT_I2C_BUS_SDA (&pin_PA04)

#define DEFAULT_SPI_BUS_SCK (&pin_PA11)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA10)
#define DEFAULT_SPI_BUS_MISO (&pin_PA09)

#define DEFAULT_UART_BUS_RX (&pin_PA07)
#define DEFAULT_UART_BUS_TX (&pin_PA06)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
