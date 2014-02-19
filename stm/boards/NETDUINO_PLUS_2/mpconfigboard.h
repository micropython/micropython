#define NETDUINO_PLUS_2

#define MICROPY_HW_BOARD_NAME       "NetduinoPlus2"

#define MICROPY_HW_HAS_SWITCH       (1)

// On the netuino, the sdcard appears to be wired up as a 1-bit
// SPI, so the driver needs to be converted to support that before
// we can turn this on.
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_HAS_WLAN         (0)
#define MICROPY_HW_ENABLE_RNG       (1)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_SERVO     (1)
#define MICROPY_HW_ENABLE_AUDIO     (0)

#define USRSW_PORT          (GPIOB)
#define USRSW_PIN           (GPIO_Pin_11)
#define USRSW_PUPD          (GPIO_PuPd_NOPULL)
#define USRSW_EXTI_PIN      (EXTI_PinSource11)
#define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOB)
#define USRSW_EXTI_LINE     (EXTI_Line11)
#define USRSW_EXTI_IRQN     (EXTI15_10_IRQn)
#define USRSW_EXTI_EDGE     (EXTI_Trigger_Rising)

/* LED */
#define PYB_LED1_PORT   (GPIOA)         // Blue LED
#define PYB_LED1_PIN    (GPIO_Pin_10)

#define PYB_LED2_PORT   (GPIOC)         // White LED (aka Power)
#define PYB_LED2_PIN    (GPIO_Pin_13)

#define PYB_LED3_PORT   (GPIOA)         // Same as Led(1)
#define PYB_LED3_PIN    (GPIO_Pin_10)

#define PYB_LED4_PORT   (GPIOC)         // Same as Led(2)
#define PYB_LED4_PIN    (GPIO_Pin_13)

#define PYB_OTYPE       (GPIO_OType_PP)

#define PYB_LED_ON(port, pin)  (port->BSRRL = pin)
#define PYB_LED_OFF(port, pin) (port->BSRRH = pin)

#define HSE_VALUE (25000000)
