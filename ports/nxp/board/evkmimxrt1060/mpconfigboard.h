#define MICROPY_HW_BOARD_NAME "evkmimxrt1060"
#define MICROPY_HW_MCU_NAME "imxrt1062"
#define MICROPY_USE_IMXRT1060_MCU  (1)

/* Enable LVGL */
#define MICROPY_PY_LVGL             (1)
#define MICROPY_PY_PINMUX           (1)

/* LED */
#define MICROPY_PY_LED              (1)
#define LED_INFO                    { {NULL,0}, {"gpio_1", 9} }

/* GPIO */
#define MICROPY_PY_GPIO             (1)

/* Builtin UART */
#define UART_CORE_INCLUDE           "fsl_lpuart.h"
#define UART_READ_BLOCKING          LPUART_ReadBlocking
#define UART_WRITE_BLOCKING         LPUART_WriteBlocking
#define UART_INSTNACE               LPUART1

/* LPSPI */
#define MICROPY_PY_SPI              (1)
#define MP_FEATURE_SPI_COUNTS            5
#define BOARD_SPI_CMSIS_HEADER  "fsl_lpspi_cmsis.h"
#define MICROPY_HW_SPI1_NAME        "LPSPI1"
#define MICROPY_HW_SPI2_NAME        "LPSPI2"
#define MICROPY_HW_SPI3_NAME        "LPSPI3"
#define MICROPY_HW_SPI4_NAME        "LPSPI4"

/* LPUART */
#define MICROPY_PY_USART            (1)
#define MP_FEATURE_USART_COUNTS       6
#define BOARD_USART_CMSIS_HEADER "fsl_lpuart_cmsis.h"
#define MICROPY_HW_USUART1_NAME     "LPUART1"
#define MICROPY_HW_USUART2_NAME     "LPUART2"
#define MICROPY_HW_USUART3_NAME     "LPUART3"
#define MICROPY_HW_USUART4_NAME     "LPUART4"
#define MICROPY_HW_USUART5_NAME     "LPUART5"

/* LPI2C */
#define MICROPY_PY_I2C            (1)
#define MP_FEATURE_I2C_COUNTS       (5)
#define BOARD_I2C_CMSIS_HEADER  "fsl_lpi2c_cmsis.h"
#define MICROPY_HW_I2C1_NAME        "LPI2C1"
#define MICROPY_HW_I2C2_NAME        "LPI2C2"
#define MICROPY_HW_I2C3_NAME        "LPI2C3"
#define MICROPY_HW_I2C4_NAME        "LPI2C4"