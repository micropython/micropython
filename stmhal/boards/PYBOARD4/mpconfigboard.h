#define PYBOARD4

#define MICROPY_HW_BOARD_NAME       "PYBv4"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_HAS_MMA7660      (1)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (1)
#define MICROPY_HW_HAS_WLAN         (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_AUDIO     (1)

// USRSW has no pullup or pulldown, and pressing the switch makes the input go low
#define USRSW_PIN           (pin_B3)
#define USRSW_PUPD          (GPIO_PuPd_UP)
#define USRSW_EXTI_EDGE     (EXTI_Trigger_Falling)
#define USRSW_PRESSED       (0)

/* LED */
#define PYB_LED1        (pin_A13) // red
#define PYB_LED2        (pin_A14) // green
#define PYB_LED3        (pin_A15) // yellow
#define PYB_LED4        (pin_B4)  // blue

#define PYB_OTYPE       (GPIO_OType_PP)

#define PYB_LED_ON(pin)  (pin->gpio->BSRRL = pin->pin_mask)
#define PYB_LED_OFF(pin) (pin->gpio->BSRRH = pin->pin_mask)

