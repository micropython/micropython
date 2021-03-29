#define MICROPY_HW_BOARD_NAME "i.MX RT1050 EVK"
#define MICROPY_HW_MCU_NAME   "MIMXRT1052DVL6B"

#define BOARD_FLASH_SIZE (64 * 1024 * 1024)

// MIMXRT1050_EVK has 1 user LED
#define MICROPY_HW_LED1_PIN (GPIO_AD_B0_09)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_low(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_high(pin))
