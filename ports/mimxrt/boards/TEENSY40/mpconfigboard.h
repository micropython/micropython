#define MICROPY_HW_BOARD_NAME "Teensy 4.0"
#define MICROPY_HW_MCU_NAME   "MIMXRT1062DVJ6A"

#define BOARD_FLASH_SIZE (2 * 1024 * 1024)

// Teensy 4.0 has 1 board LED
#define MICROPY_HW_LED1_PIN (GPIO_B0_03)
#define MICROPY_HW_LED_ON(pin) (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin) (mp_hal_pin_low(pin))
