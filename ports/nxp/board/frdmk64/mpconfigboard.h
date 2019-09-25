#define MICROPY_HW_BOARD_NAME "frdmk64"
#define MICROPY_HW_MCU_NAME "MK64F12"
#define MICROPY_USE_MK64F12_MCU        (1)

/* Enable Built-in modules */
#define MICROPY_PY_LVGL             (0)
#define MICROPY_PY_LPI2C            (0)
#define MICROPY_PY_PINMUX           (0)

/* LED */
#define MICROPY_PY_LED              (1)
#define LED_INFO                    { {NULL, 0}, {"gpio_1", 21}, {"gpio_1", 22}, {"gpio_4", 26} }

/* GPIO */
#define MICROPY_PY_GPIO             (1)

/* Enable UART */
#define UART_CORE_INCLUDE           "fsl_uart.h"
#define UART_READ_BLOCKING          UART_ReadBlocking
#define UART_WRITE_BLOCKING         UART_WriteBlocking
#define UART_INSTNACE               UART0

/* ESPI */
#define MICROPY_PY_SPI              (1)
#define MP_FEATURE_SPI_COUNTS       3
#define BOARD_SPI_CMSIS_HEADER     "fsl_dspi_cmsis.h"
#define MICROPY_HW_SPI0_NAME       "DSPI0"
#define MICROPY_HW_SPI1_NAME       "DSPI1"
#define MICROPY_HW_SPI2_NAME       "DSPI2"

/* UART */
#define MICROPY_PY_USART            (1)
#define MP_FEATURE_USART_COUNTS       6
#define BOARD_USART_CMSIS_HEADER "fsl_uart_cmsis.h"
#define MICROPY_HW_USUART0_NAME     "UART0"
#define MICROPY_HW_USUART1_NAME     "UART1"
#define MICROPY_HW_USUART2_NAME     "UART2"
#define MICROPY_HW_USUART3_NAME     "UART3"
#define MICROPY_HW_USUART4_NAME     "UART4"
#define MICROPY_HW_USUART5_NAME     "UART5"