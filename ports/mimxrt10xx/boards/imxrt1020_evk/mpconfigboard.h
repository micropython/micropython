#define MICROPY_HW_BOARD_NAME "iMX RT 1020 EVK"
#define MICROPY_HW_MCU_NAME "IMXRT1021DAG5A"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (8 * 1024 * 1024)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO_SD_B1_03)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO_SD_B1_02)

#define DEFAULT_UART_BUS_RX (&pin_GPIO_AD_B1_09)
#define DEFAULT_UART_BUS_TX (&pin_GPIO_AD_B1_08)
