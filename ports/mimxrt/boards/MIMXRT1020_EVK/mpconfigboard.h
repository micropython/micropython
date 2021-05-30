#define MICROPY_HW_BOARD_NAME "i.MX RT1020 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1021DAG5A"

#define BOARD_FLASH_SIZE (8 * 1024 * 1024)

// i.MX RT1020 EVK has 1 board LED
// Todo: think about replacing the define with searching in the generated pins?
#define MICROPY_HW_LED1_PIN (pin_GPIO_AD_B0_05)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_high(pin))
#define BOARD_FLASH_CONFIG_HEADER_H "evkmimxrt1020_flexspi_nor_config.h"

// Define mapping logical UART # to hardware uart #
#define MICROPY_HW_UART_NUM     (sizeof(uart_index_table)/sizeof(uart_index_table)[0])
#define MICROPY_HW_UART_INDEX   { 0, 2, 3, 5, 8, 4 }
