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
#define MICROPY_HW_ENABLE_AUDIO     (0)

// USRSW has no pullup or pulldown, and pressing the switch makes the input go low
#define USRSW_PIN           (pin_A13)
#define USRSW_PUPD          (GPIO_PuPd_UP)
#define USRSW_EXTI_EDGE     (EXTI_Trigger_Falling)
#define USRSW_PRESSED       (0)

/* LED */
#define PYB_LED1_PORT   (GPIOA)
#define PYB_LED1_PIN    (GPIO_Pin_8)

#define PYB_LED2_PORT   (GPIOA)
#define PYB_LED2_PIN    (GPIO_Pin_10)

#define PYB_LED3_PORT   (GPIOC)
#define PYB_LED3_PIN    (GPIO_Pin_4)

#define PYB_LED4_PORT   (GPIOC)
#define PYB_LED4_PIN    (GPIO_Pin_5)

#define PYB_OTYPE       (GPIO_OType_OD)

#define PYB_LED_ON(port, pin)  (port->BSRRH = pin)
#define PYB_LED_OFF(port, pin) (port->BSRRL = pin)
