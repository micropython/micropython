#define MICROPY_HW_BOARD_NAME "i.MX RT1020 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1021DAG5A"

#define BOARD_FLASH_SIZE (8 * 1024 * 1024)

// i.MX RT1020 EVK has 1 board LED
#define MICROPY_HW_LED1_PIN (GPIO_AD_B0_05)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_high(pin))
