#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_REPL_HELPERS (1)
#define MICROPY_ENABLE_FLOAT        (1)
#define MICROPY_PATH_MAX            (128)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

typedef int32_t machine_int_t; // must be pointer size
typedef uint32_t machine_uint_t; // must be pointer size
typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size
typedef float machine_float_t;

machine_float_t machine_sqrt(machine_float_t x);

// board specific definitions

// choose 1 of these boards
//#define PYBOARD3
#define PYBOARD4
//#define STM32F4DISC
//#define NETDUINO_PLUS_2

#if defined (PYBOARD3)
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

    #define USRSW_PORT          (GPIOA)
    #define USRSW_PIN           (GPIO_Pin_13)
    #define USRSW_PUPD          (GPIO_PuPd_UP)
    #define USRSW_EXTI_PIN      (EXTI_PinSource13)
    #define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOA)
    #define USRSW_EXTI_LINE     (EXTI_Line13)
    #define USRSW_EXTI_IRQN     (EXTI15_10_IRQn)
    #define USRSW_EXTI_EDGE     (EXTI_Trigger_Rising)

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

#elif defined (PYBOARD4)
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

#elif defined (STM32F4DISC)
    #define MICROPY_HW_BOARD_NAME       "F4DISC"

    #define MICROPY_HW_HAS_SWITCH       (1)
    #define MICROPY_HW_HAS_SDCARD       (0)
    #define MICROPY_HW_HAS_MMA7660      (0)
    #define MICROPY_HW_HAS_LIS3DSH      (1)
    #define MICROPY_HW_HAS_LCD          (0)
    #define MICROPY_HW_HAS_WLAN         (0)
    #define MICROPY_HW_ENABLE_RNG       (1)
    #define MICROPY_HW_ENABLE_RTC       (1)
    #define MICROPY_HW_ENABLE_TIMER     (1)
    #define MICROPY_HW_ENABLE_SERVO     (0)
    #define MICROPY_HW_ENABLE_AUDIO     (0)

    #define USRSW_PORT          (GPIOA)
    #define USRSW_PIN           (GPIO_Pin_0)
    #define USRSW_PUPD          (GPIO_PuPd_NOPULL)
    #define USRSW_EXTI_PIN      (EXTI_PinSource0)
    #define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOA)
    #define USRSW_EXTI_LINE     (EXTI_Line0)
    #define USRSW_EXTI_IRQN     (EXTI0_IRQn)
    #define USRSW_EXTI_EDGE     (EXTI_Trigger_Falling)

    /* LED */
    #define PYB_LED1_PORT   (GPIOD)
    #define PYB_LED1_PIN    (GPIO_Pin_14)

    #define PYB_LED2_PORT   (GPIOD)
    #define PYB_LED2_PIN    (GPIO_Pin_12)

    #define PYB_LED3_PORT   (GPIOD)
    #define PYB_LED3_PIN    (GPIO_Pin_15)

    #define PYB_LED4_PORT   (GPIOD)
    #define PYB_LED4_PIN    (GPIO_Pin_13)

    #define PYB_OTYPE       (GPIO_OType_PP)

    #define PYB_LED_ON(port, pin)  (port->BSRRL = pin)
    #define PYB_LED_OFF(port, pin) (port->BSRRH = pin)

#elif defined (NETDUINO_PLUS_2)
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
#endif

#define STM32F40_41xxx
#define USE_STDPERIPH_DRIVER
#if !defined(HSE_VALUE)
#define HSE_VALUE (8000000)
#endif
#define USE_DEVICE_MODE
//#define USE_HOST_MODE
