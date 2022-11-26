#include <stdint.h>

// options to control how MicroPython is built

#define MICROPY_ALLOC_PATH_MAX      (128)
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_INFO           (1)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_OPT_COMPUTED_GOTO   (1)

#define MICROPY_PY_BUILTINS_INPUT   (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT teensy_help_text

#define MICROPY_PY_IO               (0)
#define MICROPY_PY_FROZENSET        (1)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_CMATH            (1)

#define MICROPY_TIMER_REG           (0)
#define MICROPY_REG                 (MICROPY_TIMER_REG)

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF   (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE  (0)

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \

// extra constants
extern const struct _mp_obj_module_t pyb_module;
#define MICROPY_PORT_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_pyb), MP_ROM_PTR(&pyb_module) }, \

#define MP_STATE_PORT MP_STATE_VM

// type definitions for the specific machine

#define UINT_FMT "%u"
#define INT_FMT "%d"

typedef int32_t mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
typedef long mp_off_t;

// We have inlined IRQ functions for efficiency (they are generally
// 1 machine instruction).
//
// Note on IRQ state: you should not need to know the specific
// value of the state variable, but rather just pass the return
// value from disable_irq back to enable_irq.  If you really need
// to know the machine-specific values, see irq.h.

__attribute__((always_inline)) static inline uint32_t __get_PRIMASK(void) {
    uint32_t result;
    __asm volatile ("MRS %0, primask" : "=r" (result));
    return result;
}

__attribute__((always_inline)) static inline void __set_PRIMASK(uint32_t priMask) {
    __asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

__attribute__((always_inline)) static inline void enable_irq(mp_uint_t state) {
    __set_PRIMASK(state);
}

__attribute__((always_inline)) static inline mp_uint_t disable_irq(void) {
    mp_uint_t state = __get_PRIMASK();
    __asm__ volatile ("CPSID i");
    return state;
}

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// The following would be from a board specific file, if one existed

#define MICROPY_HW_BOARD_NAME       "Teensy-3.1"
#define MICROPY_HW_MCU_NAME         "MK20DX256"

#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_SDCARD       (0)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LIS3DSH      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RNG       (0)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_TIMER     (0)
#define MICROPY_HW_ENABLE_SERVO     (0)
#define MICROPY_HW_ENABLE_DAC       (0)

#define MICROPY_HW_LED1             (pin_C5)
#define MICROPY_HW_LED_OTYPE        (GPIO_MODE_OUTPUT_PP)
#define MICROPY_HW_LED_ON(pin)      (pin->gpio->PSOR = pin->pin_mask)
#define MICROPY_HW_LED_OFF(pin)     (pin->gpio->PCOR = pin->pin_mask)

#if 0
// SD card detect switch
#define MICROPY_HW_SDCARD_DETECT_PIN        (pin_A8)
#define MICROPY_HW_SDCARD_DETECT_PULL       (GPIO_PULLUP)
#define MICROPY_HW_SDCARD_DETECT_PRESENT    (GPIO_PIN_RESET)
#endif

#define MICROPY_MATH_SQRT_ASM     (1)

#define MICROPY_MPHALPORT_H     "teensy_hal.h"
#define MICROPY_PIN_DEFS_PORT_H "pin_defs_teensy.h"
