#define MICROPY_HW_BOARD_NAME "iMX RT 1040 EVK"
#define MICROPY_HW_MCU_NAME "IMXRT1042XJM5B"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (8 * 1024 * 1024)

#define MICROPY_HW_LED_STATUS (&pin_GPIO_AD_B0_08)
#define MICROPY_HW_LED_STATUS_INVERTED (1)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO_AD_B1_00)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO_AD_B1_01)

#define DEFAULT_UART_BUS_RX (&pin_GPIO_AD_B1_07)
#define DEFAULT_UART_BUS_TX (&pin_GPIO_AD_B1_06)

#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO_AD_B0_12)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO_AD_B0_13)

// If you want to connect over SWD, then make sure J80 is open.
