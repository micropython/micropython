#define MICROPY_HW_BOARD_NAME "XinaBox CS11"
#define MICROPY_HW_MCU_NAME "samd21g18"

#define MICROPY_PORT_A        (0)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - 0x010000 - CIRCUITPY_INTERNAL_NVM_SIZE)

#define DEFAULT_I2C_BUS_SCL (&pin_PA23)
#define DEFAULT_I2C_BUS_SDA (&pin_PA22)

#define DEFAULT_SPI_BUS_SCK (&pin_PB11)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB10)
#define DEFAULT_SPI_BUS_MISO (&pin_PA12)

#define DEFAULT_UART_BUS_RX (&pin_PA11)
#define DEFAULT_UART_BUS_TX (&pin_PA10)

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

// Turn off pins not in the board mapping to save space. They aren't broken out.
#define IGNORE_PIN_PA00     1
#define IGNORE_PIN_PA01     1
#define IGNORE_PIN_PA02     1
#define IGNORE_PIN_PA03     1

#define IGNORE_PIN_PB08     1
#define IGNORE_PIN_PB09     1

#define IGNORE_PIN_PA04     1
#define IGNORE_PIN_PA05     1
#define IGNORE_PIN_PA06     1
#define IGNORE_PIN_PA08     1

#define IGNORE_PIN_PA13     1
#define IGNORE_PIN_PA14     1
#define IGNORE_PIN_PA15     1
#define IGNORE_PIN_PA18     1
#define IGNORE_PIN_PA20     1
#define IGNORE_PIN_PA21     1

#define IGNORE_PIN_PB22     1
#define IGNORE_PIN_PB23     1

#define IGNORE_PIN_PA27     1
#define IGNORE_PIN_PA28     1
#define IGNORE_PIN_PA30     1
#define IGNORE_PIN_PA31     1
#define IGNORE_PIN_PB02     1
#define IGNORE_PIN_PB03     1
