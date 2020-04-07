#define MICROPY_HW_BOARD_NAME "i.MX RT1064 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1064DVL6A"


// MIMXRT1064_EVK has 1 user LED
#define MICROPY_HW_LED1_PIN (GPIO_AD_B0_09)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_high(pin))
