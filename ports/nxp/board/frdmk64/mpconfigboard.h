#define MICROPY_HW_BOARD_NAME "frdmk64"
#define MICROPY_HW_MCU_NAME "MK64F12"

/* Enable Built-in modules */
#define MICROPY_PY_LVGL             (0)
#define MICROPY_PY_LPI2C            (0)
#define MICROPY_PY_GPIO             (1)
#define MICROPY_PY_PINMUX           (0)

/* Enable UART */
#define UART_CORE_INCLUDE           "fsl_uart.h"
#define UART_READ_BLOCKING          UART_ReadBlocking
#define UART_WRITE_BLOCKING         UART_WriteBlocking
#define UART_INSTNACE               UART0