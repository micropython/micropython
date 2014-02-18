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
#define MICROPY_HW_ENABLE_AUDIO     (0)

#define USRSW_PORT          (GPIOB)
#define USRSW_PIN           (GPIO_Pin_3)
#define USRSW_PUPD          (GPIO_PuPd_UP)
#define USRSW_EXTI_PIN      (EXTI_PinSource3)
#define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOB)
#define USRSW_EXTI_LINE     (EXTI_Line3)
#define USRSW_EXTI_IRQN     (EXTI3_IRQn)
#define USRSW_EXTI_EDGE     (EXTI_Trigger_Rising)

/* LED */
#define PYB_LED1_PORT   (GPIOA)
#define PYB_LED1_PIN    (GPIO_Pin_13)

#define PYB_LED2_PORT   (GPIOA)
#define PYB_LED2_PIN    (GPIO_Pin_14)

#define PYB_LED3_PORT   (GPIOA)
#define PYB_LED3_PIN    (GPIO_Pin_15)

#define PYB_LED4_PORT   (GPIOB)
#define PYB_LED4_PIN    (GPIO_Pin_4)

#define PYB_OTYPE       (GPIO_OType_PP)

#define PYB_LED_ON(port, pin)  (port->BSRRL = pin)
#define PYB_LED_OFF(port, pin) (port->BSRRH = pin)
