#define MICROPY_HW_BOARD_NAME "Arduino Zero"
#define MICROPY_HW_MCU_NAME "samd21g18"

// #define MICROPY_HW_LED_MSC  PIN_PA17 // red
#define MICROPY_HW_LED_TX   PIN_PA27
#define MICROPY_HW_LED_RX   PIN_PB03

#define MICROPY_PORT_A        (PORT_PA24 | PORT_PA25 | PORT_PA27)
#define MICROPY_PORT_B        (PORT_PB03)
#define MICROPY_PORT_C        (0)

#include "internal_flash.h"

#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - 0x010000)

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
