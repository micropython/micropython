/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "pin.h"
#include "extint.h"
#include "irq.h"

/// \moduleref pyb
/// \class ExtInt - configure I/O pins to interrupt on external events
///
/// There are a total of 22 interrupt lines. 16 of these can come from GPIO pins
/// and the remaining 6 are from internal sources.
///
/// For lines 0 thru 15, a given line can map to the corresponding line from an
/// arbitrary port. So line 0 can map to Px0 where x is A, B, C, ... and
/// line 1 can map to Px1 where x is A, B, C, ...
///
///     def callback(line):
///         print("line =", line)
///
/// Note: ExtInt will automatically configure the gpio line as an input.
///
///     extint = pyb.ExtInt(pin, pyb.ExtInt.IRQ_FALLING, pyb.Pin.PULL_UP, callback)
///
/// Now every time a falling edge is seen on the X1 pin, the callback will be
/// called. Caution: mechanical pushbuttons have "bounce" and pushing or
/// releasing a switch will often generate multiple edges.
/// See: http://www.eng.utah.edu/~cs5780/debouncing.pdf for a detailed
/// explanation, along with various techniques for debouncing.
///
/// Trying to register 2 callbacks onto the same pin will throw an exception.
///
/// If pin is passed as an integer, then it is assumed to map to one of the
/// internal interrupt sources, and must be in the range 16 thru 22.
///
/// All other pin objects go through the pin mapper to come up with one of the
/// gpio pins.
///
///     extint = pyb.ExtInt(pin, mode, pull, callback)
///
/// Valid modes are pyb.ExtInt.IRQ_RISING, pyb.ExtInt.IRQ_FALLING,
/// pyb.ExtInt.IRQ_RISING_FALLING, pyb.ExtInt.EVT_RISING,
/// pyb.ExtInt.EVT_FALLING, and pyb.ExtInt.EVT_RISING_FALLING.
///
/// Only the IRQ_xxx modes have been tested. The EVT_xxx modes have
/// something to do with sleep mode and the WFE instruction.
///
/// Valid pull values are pyb.Pin.PULL_UP, pyb.Pin.PULL_DOWN, pyb.Pin.PULL_NONE.
///
/// There is also a C API, so that drivers which require EXTI interrupt lines
/// can also use this code. See extint.h for the available functions and
/// usrsw.h for an example of using this.

// TODO Add python method to change callback object.

#define EXTI_OFFSET	(EXTI_BASE - PERIPH_BASE)

// Macro used to set/clear the bit corresponding to the line in the IMR/EMR
// register in an atomic fashion by using bitband addressing.
#define EXTI_MODE_BB(mode, line) (*(__IO uint32_t *)(PERIPH_BB_BASE + ((EXTI_OFFSET + (mode)) * 32) + ((line) * 4)))

#if defined(MCU_SERIES_L4)
// The L4 MCU supports 40 Events/IRQs lines of the type configurable and direct.
// Here we only support configurable line types.  Details, see page 330 of RM0351, Rev 1.
// The USB_FS_WAKUP event is a direct type and there is no support for it.
#define EXTI_Mode_Interrupt offsetof(EXTI_TypeDef, IMR1)
#define EXTI_Mode_Event     offsetof(EXTI_TypeDef, EMR1)
#else
#define EXTI_Mode_Interrupt offsetof(EXTI_TypeDef, IMR)
#define EXTI_Mode_Event     offsetof(EXTI_TypeDef, EMR)
#endif

#define EXTI_SWIER_BB(line) (*(__IO uint32_t *)(PERIPH_BB_BASE + ((EXTI_OFFSET + offsetof(EXTI_TypeDef, SWIER)) * 32) + ((line) * 4)))

typedef struct {
    mp_obj_base_t base;
    mp_int_t line;
} extint_obj_t;

STATIC uint32_t pyb_extint_mode[EXTI_NUM_VECTORS];

#if !defined(ETH)
#define ETH_WKUP_IRQn   62  // Some MCUs don't have ETH, but we want a value to put in our table
#endif
#if !defined(OTG_HS_WKUP_IRQn)
#define OTG_HS_WKUP_IRQn 76  // Some MCUs don't have HS, but we want a value to put in our table
#endif
#if !defined(OTG_FS_WKUP_IRQn)
#define OTG_FS_WKUP_IRQn 42  // Some MCUs don't have FS IRQ, but we want a value to put in our table
#endif

STATIC const uint8_t nvic_irq_channel[EXTI_NUM_VECTORS] = {
    EXTI0_IRQn,     EXTI1_IRQn,     EXTI2_IRQn,     EXTI3_IRQn,     EXTI4_IRQn,
    EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,   EXTI9_5_IRQn,
    EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn,
    EXTI15_10_IRQn,
    #if defined(MCU_SERIES_L4)
    PVD_PVM_IRQn,
    #else
    PVD_IRQn,
    #endif
    RTC_Alarm_IRQn,
    OTG_FS_WKUP_IRQn,
    ETH_WKUP_IRQn,
    OTG_HS_WKUP_IRQn,
    TAMP_STAMP_IRQn,
    RTC_WKUP_IRQn,
};

// Set override_callback_obj to true if you want to unconditionally set the
// callback function.
uint extint_register(mp_obj_t pin_obj, uint32_t mode, uint32_t pull, mp_obj_t callback_obj, bool override_callback_obj) {
    const pin_obj_t *pin = NULL;
    uint v_line;

    if (MP_OBJ_IS_INT(pin_obj)) {
        // If an integer is passed in, then use it to identify lines 16 thru 22
        // We expect lines 0 thru 15 to be passed in as a pin, so that we can
        // get both the port number and line number.
        v_line = mp_obj_get_int(pin_obj);
        if (v_line < 16) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ExtInt vector %d < 16, use a Pin object", v_line));
        }
        if (v_line >= EXTI_NUM_VECTORS) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ExtInt vector %d >= max of %d", v_line, EXTI_NUM_VECTORS));
        }
    } else {
        pin = pin_find(pin_obj);
        v_line = pin->pin;
    }
    if (mode != GPIO_MODE_IT_RISING &&
        mode != GPIO_MODE_IT_FALLING &&
        mode != GPIO_MODE_IT_RISING_FALLING &&
        mode != GPIO_MODE_EVT_RISING &&
        mode != GPIO_MODE_EVT_FALLING &&
        mode != GPIO_MODE_EVT_RISING_FALLING) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid ExtInt Mode: %d", mode));
    }
    if (pull != GPIO_NOPULL &&
        pull != GPIO_PULLUP &&
        pull != GPIO_PULLDOWN) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid ExtInt Pull: %d", pull));
    }

    mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[v_line];
    if (!override_callback_obj && *cb != mp_const_none && callback_obj != mp_const_none) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ExtInt vector %d is already in use", v_line));
    }

    // We need to update callback atomically, so we disable the line
    // before we update anything.

    extint_disable(v_line);

    *cb = callback_obj;
    pyb_extint_mode[v_line] = (mode & 0x00010000) ? // GPIO_MODE_IT == 0x00010000
        EXTI_Mode_Interrupt : EXTI_Mode_Event;

    if (*cb != mp_const_none) {

        mp_hal_gpio_clock_enable(pin->gpio);
        GPIO_InitTypeDef exti;
        exti.Pin = pin->pin_mask;
        exti.Mode = mode;
        exti.Pull = pull;
        exti.Speed = GPIO_SPEED_FAST;
        HAL_GPIO_Init(pin->gpio, &exti);

        // Calling HAL_GPIO_Init does an implicit extint_enable

        /* Enable and set NVIC Interrupt to the lowest priority */
        HAL_NVIC_SetPriority(nvic_irq_channel[v_line], IRQ_PRI_EXTINT, IRQ_SUBPRI_EXTINT);
        HAL_NVIC_EnableIRQ(nvic_irq_channel[v_line]);
    }
    return v_line;
}

void extint_enable(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    #if defined(MCU_SERIES_F7)
    // The Cortex-M7 doesn't have bitband support.
    mp_uint_t irq_state = disable_irq();
    if (pyb_extint_mode[line] == EXTI_Mode_Interrupt) {
        EXTI->IMR |= (1 << line);
    } else {
        EXTI->EMR |= (1 << line);
    }
    enable_irq(irq_state);
    #else
    // Since manipulating IMR/EMR is a read-modify-write, and we want this to
    // be atomic, we use the bit-band area to just affect the bit we're
    // interested in.
    EXTI_MODE_BB(pyb_extint_mode[line], line) = 1;
    #endif
}

void extint_disable(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }

    #if defined(MCU_SERIES_F7)
    // The Cortex-M7 doesn't have bitband support.
    mp_uint_t irq_state = disable_irq();
    EXTI->IMR &= ~(1 << line);
    EXTI->EMR &= ~(1 << line);
    enable_irq(irq_state);
    #else
    // Since manipulating IMR/EMR is a read-modify-write, and we want this to
    // be atomic, we use the bit-band area to just affect the bit we're
    // interested in.
    EXTI_MODE_BB(EXTI_Mode_Interrupt, line) = 0;
    EXTI_MODE_BB(EXTI_Mode_Event, line) = 0;
    #endif
}

void extint_swint(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
#if defined(MCU_SERIES_L4)
    EXTI->SWIER1 = (1 << line);
#else
    EXTI->SWIER = (1 << line);
#endif
}

/// \method line()
/// Return the line number that the pin is mapped to.
STATIC mp_obj_t extint_obj_line(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(self->line);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_line_obj, extint_obj_line);

/// \method enable()
/// Enable a disabled interrupt.
STATIC mp_obj_t extint_obj_enable(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    extint_enable(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_enable_obj, extint_obj_enable);

/// \method disable()
/// Disable the interrupt associated with the ExtInt object.
/// This could be useful for debouncing.
STATIC mp_obj_t extint_obj_disable(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    extint_disable(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_disable_obj, extint_obj_disable);

/// \method swint()
/// Trigger the callback from software.
STATIC mp_obj_t extint_obj_swint(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    extint_swint(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_swint_obj,  extint_obj_swint);

// TODO document as a staticmethod
/// \classmethod regs()
/// Dump the values of the EXTI registers.
STATIC mp_obj_t extint_regs(void) {
    #if defined(MCU_SERIES_L4)
    printf("EXTI_IMR1   %08lx\n", EXTI->IMR1);
    printf("EXTI_IMR2   %08lx\n", EXTI->IMR2);
    printf("EXTI_EMR1   %08lx\n", EXTI->EMR1);
    printf("EXTI_EMR2   %08lx\n", EXTI->EMR2);
    printf("EXTI_RTSR1  %08lx\n", EXTI->RTSR1);
    printf("EXTI_RTSR2  %08lx\n", EXTI->RTSR2);
    printf("EXTI_FTSR1  %08lx\n", EXTI->FTSR1);
    printf("EXTI_FTSR2  %08lx\n", EXTI->FTSR2);
    printf("EXTI_SWIER1 %08lx\n", EXTI->SWIER1);
    printf("EXTI_SWIER2 %08lx\n", EXTI->SWIER2);
    printf("EXTI_PR1    %08lx\n", EXTI->PR1);
    printf("EXTI_PR2    %08lx\n", EXTI->PR2);
    #else
    printf("EXTI_IMR   %08lx\n", EXTI->IMR);
    printf("EXTI_EMR   %08lx\n", EXTI->EMR);
    printf("EXTI_RTSR  %08lx\n", EXTI->RTSR);
    printf("EXTI_FTSR  %08lx\n", EXTI->FTSR);
    printf("EXTI_SWIER %08lx\n", EXTI->SWIER);
    printf("EXTI_PR    %08lx\n", EXTI->PR);
    #endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(extint_regs_fun_obj, extint_regs);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(extint_regs_obj, (mp_obj_t)&extint_regs_fun_obj);

/// \classmethod \constructor(pin, mode, pull, callback)
/// Create an ExtInt object:
///
///   - `pin` is the pin on which to enable the interrupt (can be a pin object or any valid pin name).
///   - `mode` can be one of:
///     - `ExtInt.IRQ_RISING` - trigger on a rising edge;
///     - `ExtInt.IRQ_FALLING` - trigger on a falling edge;
///     - `ExtInt.IRQ_RISING_FALLING` - trigger on a rising or falling edge.
///   - `pull` can be one of:
///     - `pyb.Pin.PULL_NONE` - no pull up or down resistors;
///     - `pyb.Pin.PULL_UP` - enable the pull-up resistor;
///     - `pyb.Pin.PULL_DOWN` - enable the pull-down resistor.
///   - `callback` is the function to call when the interrupt triggers.  The
///   callback function must accept exactly 1 argument, which is the line that
///   triggered the interrupt.
STATIC const mp_arg_t pyb_extint_make_new_args[] = {
    { MP_QSTR_pin,      MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_pull,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};
#define PYB_EXTINT_MAKE_NEW_NUM_ARGS MP_ARRAY_SIZE(pyb_extint_make_new_args)

STATIC mp_obj_t extint_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // type_in == extint_obj_type

    // parse args
    mp_arg_val_t vals[PYB_EXTINT_MAKE_NEW_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, PYB_EXTINT_MAKE_NEW_NUM_ARGS, pyb_extint_make_new_args, vals);

    extint_obj_t *self = m_new_obj(extint_obj_t);
    self->base.type = type;
    self->line = extint_register(vals[0].u_obj, vals[1].u_int, vals[2].u_int, vals[3].u_obj, false);

    return self;
}

STATIC void extint_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    extint_obj_t *self = self_in;
    mp_printf(print, "<ExtInt line=%u>", self->line);
}

STATIC const mp_map_elem_t extint_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_line),    (mp_obj_t)&extint_obj_line_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable),  (mp_obj_t)&extint_obj_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable), (mp_obj_t)&extint_obj_disable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_swint),   (mp_obj_t)&extint_obj_swint_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_regs),    (mp_obj_t)&extint_regs_obj },

    // class constants
    /// \constant IRQ_RISING - interrupt on a rising edge
    /// \constant IRQ_FALLING - interrupt on a falling edge
    /// \constant IRQ_RISING_FALLING - interrupt on a rising or falling edge
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_RISING),         MP_OBJ_NEW_SMALL_INT(GPIO_MODE_IT_RISING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_FALLING),        MP_OBJ_NEW_SMALL_INT(GPIO_MODE_IT_FALLING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_RISING_FALLING), MP_OBJ_NEW_SMALL_INT(GPIO_MODE_IT_RISING_FALLING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EVT_RISING),         MP_OBJ_NEW_SMALL_INT(GPIO_MODE_EVT_RISING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EVT_FALLING),        MP_OBJ_NEW_SMALL_INT(GPIO_MODE_EVT_FALLING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_EVT_RISING_FALLING), MP_OBJ_NEW_SMALL_INT(GPIO_MODE_EVT_RISING_FALLING) },
};

STATIC MP_DEFINE_CONST_DICT(extint_locals_dict, extint_locals_dict_table);

const mp_obj_type_t extint_type = {
    { &mp_type_type },
    .name = MP_QSTR_ExtInt,
    .print = extint_obj_print,
    .make_new = extint_make_new,
    .locals_dict = (mp_obj_t)&extint_locals_dict,
};

void extint_init0(void) {
    for (int i = 0; i < PYB_EXTI_NUM_VECTORS; i++) {
        MP_STATE_PORT(pyb_extint_callback)[i] = mp_const_none;
        pyb_extint_mode[i] = EXTI_Mode_Interrupt;
   }
}

// Interrupt handler
void Handle_EXTI_Irq(uint32_t line) {
    if (__HAL_GPIO_EXTI_GET_FLAG(1 << line)) {
        __HAL_GPIO_EXTI_CLEAR_FLAG(1 << line);
        if (line < EXTI_NUM_VECTORS) {
            mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[line];
            if (*cb != mp_const_none) {
                // When executing code within a handler we must lock the GC to prevent
                // any memory allocations.  We must also catch any exceptions.
                gc_lock();
                nlr_buf_t nlr;
                if (nlr_push(&nlr) == 0) {
                    mp_call_function_1(*cb, MP_OBJ_NEW_SMALL_INT(line));
                    nlr_pop();
                } else {
                    // Uncaught exception; disable the callback so it doesn't run again.
                    *cb = mp_const_none;
                    extint_disable(line);
                    printf("Uncaught exception in ExtInt interrupt handler line %lu\n", line);
                    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
                }
                gc_unlock();
            }
        }
    }
}
