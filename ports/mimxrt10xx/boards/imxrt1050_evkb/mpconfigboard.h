#define MICROPY_HW_BOARD_NAME "iMX RT 1050 EVKB"
#define MICROPY_HW_MCU_NAME "IMXRT1052DVL6B"

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 0

// This uses the QSPI Flash. You'll need to modify the board to access it. See
// AN12108 for instructions. https://www.nxp.com/docs/en/application-note/AN12108.pdf
// JLinkGDBServer -if SWD -device "MIMXRT1052xxx6B?BankAddr=0x60000000&Loader=QSPI"
#define BOARD_FLASH_SIZE (8 * 1024 * 1024)

#define MICROPY_HW_LED_STATUS (&pin_GPIO_AD_B0_09)
#define MICROPY_HW_LED_STATUS_INVERTED (1)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO_AD_B1_00)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO_AD_B1_01)

#define DEFAULT_UART_BUS_RX (&pin_GPIO_AD_B1_07)
#define DEFAULT_UART_BUS_TX (&pin_GPIO_AD_B1_06)

#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO_AD_B0_12)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO_AD_B0_13)

// Put host on the second USB so that the device connection powers the board.
#define CIRCUITPY_USB_DEVICE_INSTANCE 0
#define CIRCUITPY_USB_HOST_INSTANCE 1
