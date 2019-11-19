#define MICROPY_HW_BOARD_NAME "NDBit6"
#define MICROPY_HW_MCU_NAME "samd21e18"

// LED status
#define MICROPY_HW_LED_STATUS   (&pin_PA23)

// These are pins not to reset.
//#define MICROPY_PORT_A        (PORT_PA00 | PORT_PA01)
#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code.
#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define DEFAULT_I2C_BUS_SCL (&pin_PA17)
#define DEFAULT_I2C_BUS_SDA (&pin_PA16)

#define DEFAULT_SPI_BUS_SCK (&pin_PA17)
#define DEFAULT_SPI_BUS_MOSI (&pin_PA16)
#define DEFAULT_SPI_BUS_MISO (&pin_PA18)

#define DEFAULT_UART_BUS_RX (&pin_PA17)
#define DEFAULT_UART_BUS_TX (&pin_PA16)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

// Not connected
//#define IGNORE_PIN_PA13     1
//#define IGNORE_PIN_PA28     1

