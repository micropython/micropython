/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "py/runtime.h"
#include "py/gc.h"
#include "py/objlist.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "rom_map.h"
#include "pin.h"
#include "gpio.h"
#include "pybpin.h"
#include "pybextint.h"
#include "mpexception.h"
#include "interrupt.h"
#include "cc3200_asm.h"
#include "mperror.h"

/// \moduleref pyb
/// \class ExtInt - configure I/O pins to interrupt on external events
///
/// There are a maximum of 25 gpio interrupt lines.
///
/// Example callback:
///
///     def callback(line):
///         print(line.pin())
///
/// Note: ExtInt will automatically configure the gpio line as an input.
///
///     extint = pyb.ExtInt('GPIO10', pyb.ExtInt.IRQ_FALLING, pyb.GPIO.STD_PU, callback)
///
/// Now every time a falling edge is seen on the gpio pin, the callback will be
/// called. Caution: mechanical pushbuttons have "bounce" and pushing or
/// releasing a switch will often generate multiple edges.
/// See: http://www.eng.utah.edu/~cs5780/debouncing.pdf for a detailed
/// explanation, along with various techniques for debouncing.
///
/// All pin objects go through the pin mapper to come up with one of the
/// gpio pins.
///
///     extint = pyb.ExtInt(pin, mode, pull, callback)
///
/// There is also a C API, so that drivers which require EXTI interrupt lines
/// can also use this code. See pybextint.h for the available functions.

STATIC void ExecuteIntCallback (extint_obj_t *self);
STATIC void GPIOA0IntHandler (void);
STATIC void GPIOA1IntHandler (void);
STATIC void GPIOA2IntHandler (void);
STATIC void GPIOA3IntHandler (void);
STATIC void EXTI_Handler(uint port);

STATIC extint_obj_t* extint_add (uint pin_num, uint port, uint bit) {
    extint_obj_t *self = m_new_obj(extint_obj_t);

    self->port = port;
    self->bit = bit;
    self->callback = NULL;
    self->pin_num = pin_num;
    // add it to the list
    mp_obj_list_append(&MP_STATE_PORT(pyb_extint_list), self);

    return self;
}

STATIC extint_obj_t* extint_find (uint port, uint8_t bit) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(pyb_extint_list).len; i++) {
        extint_obj_t *self = (extint_obj_t *)MP_STATE_PORT(pyb_extint_list).items[i];
        if (self->port == port && self->bit == bit) {
            return self;
        }
    }
    return NULL;
}

/// \method line()
/// Return the pin number to which this external interrupt is mapped to.
STATIC mp_obj_t extint_obj_pin(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(self->pin_num);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_pin_obj, extint_obj_pin);

/// \method enable()
/// Enable a disabled interrupt.
STATIC mp_obj_t extint_obj_enable(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    extint_enable(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_enable_obj, extint_obj_enable);

/// \method disable()
/// Disable the interrupt associated with the ExtInt object.
/// This could be useful for debouncing.
STATIC mp_obj_t extint_obj_disable(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    extint_disable(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_disable_obj, extint_obj_disable);

/// \method swint()
/// Trigger the callback from software.
STATIC mp_obj_t extint_obj_swint(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    extint_swint(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_swint_obj,  extint_obj_swint);

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

STATIC mp_obj_t extint_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // parse args
    mp_arg_val_t vals[PYB_EXTINT_MAKE_NEW_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, PYB_EXTINT_MAKE_NEW_NUM_ARGS, pyb_extint_make_new_args, vals);

    extint_obj_t *self = extint_register(vals[0].u_obj, vals[1].u_int, vals[2].u_int, vals[3].u_obj);
    self->base.type = type_in;

    return self;
}

STATIC void extint_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    extint_obj_t *self = self_in;
    print(env, "<ExtInt pin=%u>", self->pin_num);
}

STATIC const mp_map_elem_t extint_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_pin),                 (mp_obj_t)&extint_obj_pin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable),              (mp_obj_t)&extint_obj_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable),             (mp_obj_t)&extint_obj_disable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_swint),               (mp_obj_t)&extint_obj_swint_obj },

    // class constants
    /// \constant IRQ_RISING            - interrupt on a rising edge
    /// \constant IRQ_FALLING           - interrupt on a falling edge
    /// \constant IRQ_RISING_FALLING    - interrupt on a rising or falling edge
    /// \constant IRQ_LOW_LEVEL         - interrupt on a low level
    /// \constant IRQ_HIGH_LEVEL        - interrupt on a high level
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_FALLING),         MP_OBJ_NEW_SMALL_INT(GPIO_FALLING_EDGE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_RISING),          MP_OBJ_NEW_SMALL_INT(GPIO_RISING_EDGE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_RISING_FALLING),  MP_OBJ_NEW_SMALL_INT(GPIO_BOTH_EDGES) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_LOW_LEVEL),       MP_OBJ_NEW_SMALL_INT(GPIO_LOW_LEVEL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_HIGH_LEVEL),      MP_OBJ_NEW_SMALL_INT(GPIO_HIGH_LEVEL) },
};

STATIC MP_DEFINE_CONST_DICT(extint_locals_dict, extint_locals_dict_table);

STATIC void ExecuteIntCallback (extint_obj_t *self) {
    if (self->callback != mp_const_none) {
        // disable interrupts to avoid nesting
        uint primsk = disable_irq();
        // when executing code within a handler we must lock the GC to prevent
        // any memory allocations.  We must also catch any exceptions.
        gc_lock();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_call_function_1(self->callback, self);
            nlr_pop();
        } else {
            // uncaught exception; disable the callback so it doesn't run again
            self->callback = mp_const_none;
            extint_disable(self);
            // printing an exception here will cause a stack overflow that ends up in a
            // hard fault so, is better to signal the uncaught (probably non-recoverable)
            // exception by blinkg the system led
            mperror_signal_error();
        }
        gc_unlock();
        enable_irq(primsk);
    }
}

STATIC void GPIOA0IntHandler (void) {
    EXTI_Handler(GPIOA0_BASE);
}

STATIC void GPIOA1IntHandler (void) {
    EXTI_Handler(GPIOA1_BASE);
}

STATIC void GPIOA2IntHandler (void) {
    EXTI_Handler(GPIOA2_BASE);
}

STATIC void GPIOA3IntHandler (void) {
    EXTI_Handler(GPIOA3_BASE);
}

// common interrupt handler
STATIC void EXTI_Handler(uint port) {
    extint_obj_t *self;
    uint32_t bit = MAP_GPIOIntStatus(port, true);

    MAP_GPIOIntClear(port, bit);
    if (NULL != (self = extint_find(port, bit))) {
        ExecuteIntCallback(self);
    }
}

const mp_obj_type_t extint_type = {
    { &mp_type_type },
    .name = MP_QSTR_ExtInt,
    .print = extint_obj_print,
    .make_new = extint_make_new,
    .locals_dict = (mp_obj_t)&extint_locals_dict,
};

void extint_init0(void) {
    mp_obj_list_init(&MP_STATE_PORT(pyb_extint_list), 0);
}

extint_obj_t* extint_register(mp_obj_t pin_obj, uint32_t intmode, uint32_t pull, mp_obj_t callback) {
    const pin_obj_t *pin;
    extint_obj_t* self;
    void *handler;
    uint32_t intnum;

    pin = pin_find(pin_obj);

    if (intmode != GPIO_FALLING_EDGE &&
        intmode != GPIO_RISING_EDGE &&
        intmode != GPIO_BOTH_EDGES &&
        intmode != GPIO_LOW_LEVEL &&
        intmode != GPIO_HIGH_LEVEL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
    if (pull != PIN_TYPE_STD &&
        pull != PIN_TYPE_STD_PU &&
        pull != PIN_TYPE_STD_PD &&
        pull != PIN_TYPE_OD &&
        pull != PIN_TYPE_OD_PU &&
        pull != PIN_TYPE_OD_PD) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

    if (NULL == (self = extint_find(pin->port, pin->bit))) {
        self = extint_add(pin->pin_num, pin->port, pin->bit);
    }
    else {
        // we need to update the callback atomically, so we disable the line
        // before we update anything.
        extint_disable(self);
    }

    // invalidate the callback
    self->callback = NULL;

    // before enabling the interrupt, configure the gpio pin
    pin_config(pin, PIN_MODE_0, GPIO_DIR_MODE_IN, pull, PIN_STRENGTH_4MA);

    MAP_GPIOIntTypeSet(pin->port, pin->bit, intmode);
    switch (pin->port) {
    case GPIOA0_BASE:
        handler = GPIOA0IntHandler;
        intnum = INT_GPIOA0;
        break;
    case GPIOA1_BASE:
        handler = GPIOA1IntHandler;
        intnum = INT_GPIOA1;
        break;
    case GPIOA2_BASE:
        handler = GPIOA2IntHandler;
        intnum = INT_GPIOA2;
        break;
    case GPIOA3_BASE:
    default:
        handler = GPIOA3IntHandler;
        intnum = INT_GPIOA3;
        break;
    }

    MAP_GPIOIntRegister(pin->port, handler);
    // set the interrupt to the lowest priority, to make sure that no ther
    // isr will be preemted by this one
    MAP_IntPrioritySet(intnum, INT_PRIORITY_LVL_7);

    // set the new callback
    self->callback = callback;
    // enable the interrupt just before leaving
    extint_enable(self);

    return self;
}

void extint_enable(extint_obj_t *self) {
    MAP_GPIOIntClear(self->port, self->bit);
    MAP_GPIOIntEnable(self->port, self->bit);
}

void extint_disable(extint_obj_t *self) {
    MAP_GPIOIntDisable(self->port, self->bit);
}

void extint_swint(extint_obj_t *self) {
    ExecuteIntCallback(self);
}
