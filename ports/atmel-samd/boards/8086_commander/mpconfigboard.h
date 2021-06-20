#define MICROPY_HW_BOARD_NAME "8086 Commander"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define BOARD_HAS_CRYSTAL 0
#define CALIBRATE_CRYSTALLESS 1

#define MICROPY_HW_LED_STATUS   (&pin_PA06)

#define MICROPY_PORT_A          (0)
#define MICROPY_PORT_B          (0)
#define MICROPY_PORT_C          (0)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)

#define SPI_FLASH_BAUDRATE  (8000000)

#define SPI_FLASH_MISO_PIN          &pin_PA19
#define SPI_FLASH_MOSI_PIN          &pin_PA16
#define SPI_FLASH_SCK_PIN           &pin_PA17
#define SPI_FLASH_CS_PIN            &pin_PA18

#define DEFAULT_UART_BUS_RX (&pin_PA11)
#define DEFAULT_UART_BUS_TX (&pin_PA10)
