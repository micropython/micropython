#define PYBOARD3

#define MICROPY_HW_BOARD_NAME       "PYBv3"

#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_SDCARD       (1)
#define MICROPY_HW_HAS_MMA7660      (1)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_HAS_WLAN         (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_DAC       (0)

// USRSW has no pullup or pulldown, and pressing the switch makes the input go low
#define USRSW_PIN           (pin_A13)
#define USRSW_PULL          (GPIO_PULLUP)
#define USRSW_EXTI_MODE     (GPIO_MODE_IT_FALLING)
#define USRSW_PRESSED       (0)

/* LED */
#define PYB_LED1        (pin_A8)  // R1 - red
#define PYB_LED2        (pin_A10) // R2 - red
#define PYB_LED3        (pin_C4)  // G1 - green
#define PYB_LED4        (pin_C5)  // G2 - green

#define PYB_OTYPE       (GPIO_MODE_OUTPUT_PP)

#define PYB_LED_ON(pin)  (pin->gpio->BSRRH = pin->pin_mask)
#define PYB_LED_OFF(pin) (pin->gpio->BSRRL = pin->pin_mask)
