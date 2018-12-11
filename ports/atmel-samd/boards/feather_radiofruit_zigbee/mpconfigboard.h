#define MICROPY_HW_BOARD_NAME "Adafruit Feather RadioFruit Zigbee"
#define MICROPY_HW_MCU_NAME "samr21g18"

#define MICROPY_HW_LED_STATUS   (&pin_PA27)
#define MICROPY_HW_NEOPIXEL (&pin_PA22)

#define SPI_FLASH_MOSI_PIN          &pin_PA31
#define SPI_FLASH_MISO_PIN          &pin_PA30
#define SPI_FLASH_SCK_PIN           &pin_PA17
#define SPI_FLASH_CS_PIN            &pin_PA28

// These are pins not to reset.
#define MICROPY_PORT_A        (PORT_PA22)
#define MICROPY_PORT_B        ( 0 )
#define MICROPY_PORT_C        ( 0 )

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA13)
#define DEFAULT_I2C_BUS_SDA (&pin_PA12)

#define DEFAULT_SPI_BUS_SCK (&pin_PB23)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB22)
#define DEFAULT_SPI_BUS_MISO (&pin_PA23)

#define DEFAULT_UART_BUS_RX (&pin_PA09)
#define DEFAULT_UART_BUS_TX (&pin_PA08)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
