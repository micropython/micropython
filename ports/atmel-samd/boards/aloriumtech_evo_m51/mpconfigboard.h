#define MICROPY_HW_BOARD_NAME "AloriumTech Evo M51"
#define MICROPY_HW_MCU_NAME "samd51p19"

#define CIRCUITPY_MCU_FAMILY samd51

// Rev E

#define MICROPY_HW_LED_STATUS   (&pin_PA23)
#define MICROPY_HW_NEOPIXEL (&pin_PB03)

// These are pins not to reset.
// QSPI Data pins
#define MICROPY_PORT_A (PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11)
// QSPI CS, QSPI SCK and NeoPixel pin
#define MICROPY_PORT_B (PORT_PB03 | PORT_PB10 | PORT_PB11 | PORT_PB20)
#define MICROPY_PORT_C (0)
#define MICROPY_PORT_D (0)

// BC needed?
// #define AUTORESET_DELAY_MS 500

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
// #define CIRCUITPY_INTERNAL_NVM_SIZE 8192

// #define BOARD_FLASH_SIZE (FLASH_SIZE - 0x4000 - CIRCUITPY_INTERNAL_NVM_SIZE)
// End BC

#define EXTERNAL_FLASH_QSPI_DUAL

#define BOARD_HAS_CRYSTAL 1

// #define DEFAULT_I2C_BUS_SCL (&pin_PA13)
// #define DEFAULT_I2C_BUS_SDA (&pin_PA12)
#define DEFAULT_I2C_BUS_SCL (&pin_PA13)
#define DEFAULT_I2C_BUS_SDA (&pin_PA12)

#define DEFAULT_SPI_BUS_SCK (&pin_PA17)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB23)
#define DEFAULT_SPI_BUS_MISO (&pin_PB22)

#define DEFAULT_UART_BUS_RX (&pin_PB17)
#define DEFAULT_UART_BUS_TX (&pin_PB16)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
