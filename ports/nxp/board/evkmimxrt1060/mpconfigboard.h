#define MICROPY_HW_BOARD_NAME "evkmimxrt1060"
#define MICROPY_HW_MCU_NAME "imxrt1062"

/* Enable LVGL */
#define MICROPY_PY_LVGL             (1)
#define MICROPY_PY_LPI2C            (1)
#define MICROPY_PY_PINMUX           (1)
#define MICROPY_PY_LED              (1)

/* Enable UART */
#define UART_CORE_INCLUDE           "fsl_lpuart.h"
#define UART_READ_BLOCKING          LPUART_ReadBlocking
#define UART_WRITE_BLOCKING         LPUART_WriteBlocking
#define UART_INSTNACE               LPUART1