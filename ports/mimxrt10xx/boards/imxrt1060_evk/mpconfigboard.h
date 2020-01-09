#define MICROPY_HW_BOARD_NAME "iMX RT 1060 EVK"
#define MICROPY_HW_MCU_NAME "IMXRT1062DVJ6A"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (8 * 1024 * 1024)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO_AD_B1_00)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO_AD_B1_01)

#define DEFAULT_UART_BUS_RX (&pin_GPIO_AD_B1_07)
#define DEFAULT_UART_BUS_TX (&pin_GPIO_AD_B1_06)
