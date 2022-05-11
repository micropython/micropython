/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021, 2022 Renesas Electronics Corporation
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

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "pendsv.h"
#include "pin.h"
#include "extint.h"
#include "irq.h"
#if defined(RA4M1) | defined(RA4M3) | defined(RA4W1) | defined(RA6M1) | defined(RA6M2) | defined(RA6M3)
#include "ra_icu.h"
#endif

/// \moduleref pyb
/// \class ExtInt - configure I/O pins to interrupt on external events
///
/// There are a total of 16 interrupt irq_nos. These can come from GPIO pins.
///
/// For irq_nos 0 thru 15, a given irq_no can map to the corresponding irq_no from an
/// arbitrary port.
///
///     def callback(irq_no):
///         print("irq_no =", irq_no)
///
/// Note: ExtInt will automatically configure the gpio irq_no as an input.
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
/// internal interrupt sources, and must be in the range 16.
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
///
/// Valid pull values are pyb.Pin.PULL_UP, pyb.Pin.PULL_NONE.
///
/// There is also a C API, so that drivers which require EXTI interrupt irq_nos
/// can also use this code. See extint.h for the available functions and
/// usrsw.h for an example of using this.

// TODO Add python method to change callback object.


typedef struct {
    mp_obj_base_t base;
    mp_int_t pin_idx;
    mp_int_t irq_no;
} extint_obj_t;

STATIC uint8_t pyb_extint_mode[EXTI_NUM_VECTORS];
STATIC bool pyb_extint_hard_irq[EXTI_NUM_VECTORS];

// The callback arg is a small-int or a ROM Pin object, so no need to scan by GC
mp_obj_t pyb_extint_callback_arg[EXTI_NUM_VECTORS];
uint extint_irq_no[EXTI_NUM_VECTORS];

void extint_callback(void *param) {
    uint irq_no = *((uint *)param);
    mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[irq_no];
    if (*cb != mp_const_none) {
        mp_sched_lock();
        // When executing code within a handler we must lock the GC to prevent
        // any memory allocations.  We must also catch any exceptions.
        gc_lock();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_call_function_1(*cb, pyb_extint_callback_arg[irq_no]);
            nlr_pop();
        } else {
            // Uncaught exception; disable the callback so it doesn't run again.
            *cb = mp_const_none;
            ra_icu_disable_irq_no(irq_no);
            printf("Uncaught exception in ExtInt interrupt handler line %u\n", (unsigned int)irq_no);
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }
        gc_unlock();
        mp_sched_unlock();
    }
}

// Set override_callback_obj to true if you want to unconditionally set the
// callback function.
uint extint_register(mp_obj_t pin_obj, uint32_t mode, uint32_t pull, mp_obj_t callback_obj, bool override_callback_obj) {
    const machine_pin_obj_t *pin = NULL;
    uint pin_idx;
    uint8_t v_line = 0xff;
    if (mp_obj_is_int(pin_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin object is not specified"));
    } else {
        pin = machine_pin_find(pin_obj);
        pin_idx = pin->pin;
        bool find = ra_icu_find_irq_no((uint32_t)pin_idx, (uint8_t *)&v_line);
        if (!find) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("The Pin object(%d) doesn't have EXTINT feature"), pin_idx);
        }
    }
    if (pull != GPIO_NOPULL &&
        pull != GPIO_PULLUP) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid ExtInt Pull: %d"), pull);
    }
    mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[v_line];
    if (!override_callback_obj && *cb != mp_const_none && callback_obj != mp_const_none) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("ExtInt vector %d is already in use"), v_line);
    }

    // We need to update callback atomically, so we disable the line
    // before we update anything.

    extint_disable(v_line);

    *cb = callback_obj;
    // ToDo: mode should be handled
    pyb_extint_mode[v_line] = mode;
    pyb_extint_callback_arg[v_line] = MP_OBJ_NEW_SMALL_INT(v_line);
    if (*cb != mp_const_none) {
        pyb_extint_callback_arg[v_line] = MP_OBJ_NEW_SMALL_INT(v_line);
        pyb_extint_hard_irq[v_line] = true;

        if (pin == NULL) {
            // pin will be NULL for non GPIO EXTI lines
            extint_trigger_mode(v_line, mode);
            extint_enable(v_line);
        } else {
            extint_irq_no[v_line] = (uint)v_line;
            ra_icu_set_callback((uint8_t)v_line, (ICU_CB)extint_callback, (void *)&extint_irq_no[v_line]);
            ra_icu_set_pin(pin_idx, true, true);
            ra_icu_enable_pin(pin_idx);
            extint_trigger_mode(v_line, mode);
            extint_enable(v_line);
        }
    }
    return v_line;
}

// This function is intended to be used by the Pin.irq() method
void extint_register_pin(const machine_pin_obj_t *pin, uint32_t mode, bool hard_irq, mp_obj_t callback_obj) {
    uint32_t line = 0;

    bool find = ra_icu_find_irq_no((uint32_t)pin->pin, (uint8_t *)&line);
    if (!find) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("The Pin object(%d) doesn't have EXTINT feature"), (uint32_t)pin->pin);
    }

    // Check if the ExtInt line is already in use by another Pin/ExtInt
    mp_obj_t *cb = &MP_STATE_PORT(pyb_extint_callback)[line];
    if (*cb != mp_const_none && MP_OBJ_FROM_PTR(pin) != pyb_extint_callback_arg[line]) {
        if (mp_obj_is_small_int(pyb_extint_callback_arg[line])) {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("ExtInt vector %d is already in use"), line);
        } else {
            const machine_pin_obj_t *other_pin = MP_OBJ_TO_PTR(pyb_extint_callback_arg[line]);
            mp_raise_msg_varg(&mp_type_OSError,
                MP_ERROR_TEXT("IRQ resource already taken by Pin('%q')"), other_pin->name);
        }
    }

    extint_disable(line);

    *cb = callback_obj;
    // ToDo: mode should be handled
    pyb_extint_mode[line] = mode;

    if (*cb != mp_const_none) {
        // Configure and enable the callback

        pyb_extint_hard_irq[line] = hard_irq;
        pyb_extint_callback_arg[line] = MP_OBJ_FROM_PTR(pin);

        extint_trigger_mode(line, mode);

        // Configure the NVIC
        ra_icu_priority_irq_no((uint8_t)line, (uint32_t)IRQ_PRI_EXTINT);
        extint_irq_no[line] = (uint)line;
        ra_icu_set_callback((uint8_t)line, (ICU_CB)extint_callback, (void *)&extint_irq_no[line]);

        // Enable the interrupt
        extint_enable(line);
    }
}

void extint_enable(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    mp_uint_t irq_state = disable_irq();
    ra_icu_enable_irq_no((uint8_t)line);
    enable_irq(irq_state);
}

void extint_disable(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    mp_uint_t irq_state = disable_irq();
    ra_icu_disable_irq_no((uint8_t)line);
    enable_irq(irq_state);
}

void extint_swint(uint line) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    ra_icu_swint((uint8_t)line);
}

void extint_trigger_mode(uint line, uint32_t mode) {
    if (line >= EXTI_NUM_VECTORS) {
        return;
    }
    mp_uint_t irq_state = disable_irq();

    // cond: 0: falling, 1: rising, 2: both edge, 3 low level
    // Enable or disable the rising detector
    uint32_t cond = 0;
    if ((mode == GPIO_MODE_IT_RISING) || (mode == GPIO_MODE_EVT_RISING)) {
        cond = 1;
    } else if ((mode == GPIO_MODE_IT_FALLING) || (mode == GPIO_MODE_EVT_FALLING)) {
        cond = 0;
    } else if ((mode == GPIO_MODE_IT_RISING_FALLING) || (mode == GPIO_MODE_EVT_RISING_FALLING)) {
        cond = 2;
    } else if (mode == GPIO_IRQ_LOWLEVEL) {
        cond = 3;
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("The device doesn't have (%d) feature"), (uint32_t)mode);
    }
    ra_icu_trigger_irq_no((uint8_t)line, cond);
    enable_irq(irq_state);
}

/// \method irq_no()
/// Return the irq_no number that the pin is mapped to.
STATIC mp_obj_t extint_obj_irq_no(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t irq_no;
    bool find = ra_icu_find_irq_no(self->pin_idx, &irq_no);
    if (find) {
        return MP_OBJ_NEW_SMALL_INT(self->irq_no);
    } else {
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_irq_no_obj, extint_obj_irq_no);

/// \method enable()
/// Enable a disabled interrupt.
STATIC mp_obj_t extint_obj_enable(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ra_icu_enable_pin(self->pin_idx);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_enable_obj, extint_obj_enable);

/// \method disable()
/// Disable the interrupt associated with the ExtInt object.
/// This could be useful for debouncing.
STATIC mp_obj_t extint_obj_disable(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ra_icu_disable_pin(self->pin_idx);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_disable_obj, extint_obj_disable);

/// \method swint()
/// Trigger the callback from software.
STATIC mp_obj_t extint_obj_swint(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ra_icu_swint(self->irq_no);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_swint_obj,  extint_obj_swint);

// TODO document as a staticmethod
/// \classmethod regs()
/// Dump the values of the EXTI registers.
STATIC mp_obj_t extint_regs(void) {
    printf("Not Implemented\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(extint_regs_fun_obj, extint_regs);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(extint_regs_obj, MP_ROM_PTR(&extint_regs_fun_obj));

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
///   callback function must accept exactly 1 argument, which is the irq_no that
///   triggered the interrupt.
STATIC const mp_arg_t pyb_extint_make_new_args[] = {
    { MP_QSTR_pin,      MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_pull,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};
#define PYB_EXTINT_MAKE_NEW_NUM_ARGS MP_ARRAY_SIZE(pyb_extint_make_new_args)

STATIC mp_obj_t extint_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // type_in == extint_obj_type

    // parse args
    mp_arg_val_t vals[PYB_EXTINT_MAKE_NEW_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, PYB_EXTINT_MAKE_NEW_NUM_ARGS, pyb_extint_make_new_args, vals);

    extint_obj_t *self = mp_obj_malloc(extint_obj_t, type);
    machine_pin_obj_t *pin = vals[0].u_obj;
    self->pin_idx = pin->pin;
    self->irq_no = extint_register(vals[0].u_obj, vals[1].u_int, vals[2].u_int, vals[3].u_obj, false);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void extint_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ExtInt irq_no=%u>", self->irq_no);
}

STATIC const mp_rom_map_elem_t extint_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_irq_no),  MP_ROM_PTR(&extint_obj_irq_no_obj) },
    { MP_ROM_QSTR(MP_QSTR_line),  MP_ROM_PTR(&extint_obj_irq_no_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),  MP_ROM_PTR(&extint_obj_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&extint_obj_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_swint),   MP_ROM_PTR(&extint_obj_swint_obj) },
    { MP_ROM_QSTR(MP_QSTR_regs),    MP_ROM_PTR(&extint_regs_obj) },

    // class constants
    /// \constant IRQ_RISING - interrupt on a rising edge
    /// \constant IRQ_FALLING - interrupt on a falling edge
    /// \constant IRQ_RISING_FALLING - interrupt on a rising or falling edge
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING),         MP_ROM_INT(GPIO_MODE_IT_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING),        MP_ROM_INT(GPIO_MODE_IT_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING_FALLING), MP_ROM_INT(GPIO_MODE_IT_RISING_FALLING) },
};

STATIC MP_DEFINE_CONST_DICT(extint_locals_dict, extint_locals_dict_table);

const mp_obj_type_t extint_type = {
    { &mp_type_type },
    .name = MP_QSTR_ExtInt,
    .print = extint_obj_print,
    .make_new = extint_make_new,
    .locals_dict = (mp_obj_dict_t *)&extint_locals_dict,
};

void extint_init0(void) {
    ra_icu_init();
    ra_icu_deinit();
    for (int i = 0; i < PYB_EXTI_NUM_VECTORS; i++) {
        MP_STATE_PORT(pyb_extint_callback)[i] = mp_const_none;
    }
}
