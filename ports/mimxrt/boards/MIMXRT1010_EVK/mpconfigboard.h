#define MICROPY_HW_BOARD_NAME "i.MX RT1010 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1011DAE5A"

#define BOARD_FLASH_SIZE (16 * 1024 * 1024)

// i.MX RT1010 EVK has 1 board LED
#define MICROPY_HW_LED1_PIN (GPIO_11)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))
