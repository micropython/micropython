#define MICROPY_HW_BOARD_NAME "ndGarage[n°] Bit6: FeatherSnow-v2"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_HW_LED_STATUS   (&pin_PA23)

#define MICROPY_PORT_A        (PORT_PA24 | PORT_PA25)
#define MICROPY_PORT_B        (0)
#define MICROPY_PORT_C        (0)

#define CIRCUITPY_INTERNAL_NVM_SIZE 256

#define BOARD_FLASH_SIZE (0x00040000 - 0x2000 - 0x010000 CIRCUITPY_INTERNAL_NVM_SIZE)

#define DEFAULT_I2C_BUS_SCL (&pin_PA17)
#define DEFAULT_I2C_BUS_SDA (&pin_PA16)

#define DEFAULT_UART_BUS_RX (&pin_PA17)
#define DEFAULT_UART_BUS_TX (&pin_PA16)

// USB is always used.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1
