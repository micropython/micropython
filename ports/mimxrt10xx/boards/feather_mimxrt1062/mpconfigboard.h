#define MICROPY_HW_BOARD_NAME "Feather MIMXRT1062"
#define MICROPY_HW_MCU_NAME "IMXRT1062DVJ6A"

//TODO
//#define MICROPY_HW_LED_STATUS   (&pin_PA27)

//#define MICROPY_HW_NEOPIXEL (&pin_PB22)

// These are pins not to reset.
// QSPI Data pins
//#define MICROPY_PORT_A ( PORT_PA08 | PORT_PA09 | PORT_PA10 | PORT_PA11 )
// QSPI CS, and QSPI SCK
//#define MICROPY_PORT_B ( PORT_PB10 | PORT_PB11 | PORT_PB22 )
//#define MICROPY_PORT_C ( 0 )
//#define MICROPY_PORT_D ( 0 )

// If you change this, then make sure to update the linker scripts as well to
// make sure you don't overwrite code
#define CIRCUITPY_INTERNAL_NVM_SIZE 0

#define BOARD_FLASH_SIZE (8 * 1024 * 1024)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO_EMC_22)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO_EMC_21)

#define DEFAULT_SPI_BUS_SCK  (&pin_GPIO_B1_07)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO_B1_06)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO_B1_05)

#define DEFAULT_UART_BUS_RX (&pin_GPIO_B1_03)
#define DEFAULT_UART_BUS_TX (&pin_GPIO_B1_02)
