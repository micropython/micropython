#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_REPL_HELPERS (1)
#define MICROPY_ENABLE_FLOAT        (1)

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
//#define PYBOARD
#define PYBOARD4
//#define STM32F4DISC

#if defined (PYBOARD)
    #define __HAVE_SWITCH       (1)
    #define __HAVE_SDCARD       (1)
    #define __HAVE_MMA7660      (1)
    #define __HAVE_LIS3DSH      (0)
    #define __ENABLE_RNG        (1)
    #define __ENABLE_RTC        (1)
    #define __ENABLE_TIMER      (1)
    #define __ENABLE_SERVO      (1)
    #define __ENABLE_AUDIO      (0)

    #define USRSW_PORT          (GPIOA)
    #define USRSW_PIN           (GPIO_Pin_13)
    #define USRSW_PUPD          (GPIO_PuPd_UP)
    #define USRSW_EXTI_PIN      (EXTI_PinSource13)
    #define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOA)
    #define USRSW_EXTI_LINE     (EXTI_Line13)
    #define USRSW_EXTI_IRQN     (EXTI15_10_IRQn)
    #define USRSW_EXTI_EDGE     (EXTI_Trigger_Rising)
#elif defined (PYBOARD4)
    #define __HAVE_SWITCH       (1)
    #define __HAVE_SDCARD       (1)
    #define __HAVE_MMA7660      (1)
    #define __HAVE_LIS3DSH      (0)
    #define __ENABLE_RNG        (1)
    #define __ENABLE_RTC        (1)
    #define __ENABLE_TIMER      (1)
    #define __ENABLE_SERVO      (1)
    #define __ENABLE_AUDIO      (0)

    #define USRSW_PORT          (GPIOB)
    #define USRSW_PIN           (GPIO_Pin_3)
    #define USRSW_PUPD          (GPIO_PuPd_UP)
    #define USRSW_EXTI_PIN      (EXTI_PinSource3)
    #define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOB)
    #define USRSW_EXTI_LINE     (EXTI_Line3)
    #define USRSW_EXTI_IRQN     (EXTI15_10_IRQn)
    #define USRSW_EXTI_EDGE     (EXTI_Trigger_Rising)
#elif defined (STM32F4DISC)
    #define __HAVE_SWITCH       (1)
    #define __HAVE_SDCARD       (0)
    #define __HAVE_MMA7660      (0)
    #define __HAVE_LIS3DSH      (1)
    #define __ENABLE_RNG        (1)
    #define __ENABLE_RTC        (1)
    #define __ENABLE_TIMER      (1)
    #define __ENABLE_SERVO      (0)
    #define __ENABLE_AUDIO      (0)

    #define USRSW_PORT          (GPIOA)
    #define USRSW_PIN           (GPIO_Pin_0)
    #define USRSW_PUPD          (GPIO_PuPd_NOPULL)
    #define USRSW_EXTI_PIN      (EXTI_PinSource0)
    #define USRSW_EXTI_PORT     (EXTI_PortSourceGPIOA)
    #define USRSW_EXTI_LINE     (EXTI_Line0)
    #define USRSW_EXTI_IRQN     (EXTI0_IRQn)
    #define USRSW_EXTI_EDGE     (EXTI_Trigger_Falling)
#endif

#define STM32F40_41xxx
#define USE_STDPERIPH_DRIVER
#define HSE_VALUE (8000000)
