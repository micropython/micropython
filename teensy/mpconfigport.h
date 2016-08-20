#pragma once
#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_ALLOC_PATH_MAX      (128)
#define MICROPY_EMIT_THUMB          (1)
#define MICROPY_EMIT_INLINE_THUMB   (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_OPT_COMPUTED_GOTO   (1)

#define MICROPY_STREAMS_NON_BLOCK   (1)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)

#define MICROPY_PY_IO               (0)
#define MICROPY_PY_FROZENSET        (1)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_CMATH            (1)
#define MICROPY_PY_MACHINE          (1)

#define MICROPY_MATH_SQRT_ASM       (1)

// The following should eventually be replaced by the machine.mem when we
// get constants generated.
#define MICROPY_TIMER_REG           (0)
#define MICROPY_REG                 (MICROPY_TIMER_REG)

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF   (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE  (0)

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_help), (mp_obj_t)&mp_builtin_help_obj }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj }, \

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t machine_module;
extern const struct _mp_obj_module_t os_module;
extern const struct _mp_obj_module_t pyb_module;
extern const struct _mp_obj_module_t time_module;
#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_umachine), (mp_obj_t)&machine_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_pyb), (mp_obj_t)&pyb_module }, \

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_machine), (mp_obj_t)&machine_module }, \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_umachine), (mp_obj_t)&machine_module }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_pyb), (mp_obj_t)&pyb_module }, \

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    mp_obj_t pin_class_mapper; \
    mp_obj_t pin_class_map_dict; \
    struct _pyb_uart_obj_t *pyb_stdio_uart; \
    /* pointers to all UART objects (if they have been created) */ \
    struct _pyb_uart_obj_t *pyb_uart_obj_all[MICROPY_HW_NUM_UARTS]; \

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define UINT_FMT "%u"
#define INT_FMT "%d"

typedef int32_t mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// We have inlined IRQ functions for efficiency (they are generally
// 1 machine instruction).
//
// Note on IRQ state: you should not need to know the specific
// value of the state variable, but rather just pass the return
// value from disable_irq back to enable_irq.  If you really need
// to know the machine-specific values, see irq.h.

#ifndef __disable_irq
#define __disable_irq() __asm__ volatile("CPSID i":::"memory");
#endif

__attribute__(( always_inline )) static inline uint32_t __get_PRIMASK(void) {
    uint32_t result;
    __asm volatile ("MRS %0, primask" : "=r" (result));
    return(result);
}

__attribute__(( always_inline )) static inline void __set_PRIMASK(uint32_t priMask) {
    __asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

__attribute__(( always_inline )) static inline void enable_irq(mp_uint_t state) {
    __set_PRIMASK(state);
}

__attribute__(( always_inline )) static inline mp_uint_t disable_irq(void) {
    mp_uint_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

#include "mpconfigboard.h"

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_MPHALPORT_H     "teensy_hal.h"
#define MICROPY_PIN_DEFS_PORT_H "pin_defs_teensy.h"

#endif // __INCLUDED_MPCONFIGPORT_H
