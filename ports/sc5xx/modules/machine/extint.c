/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019 Analog Devices, Inc.
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
#include "pin.h"
#include "extint.h"

#include <services/gpio/adi_gpio.h>
#include <services/int/adi_int.h>
#include <sys/platform.h>

typedef struct {
    mp_obj_base_t base;
    mp_int_t line;
} extint_obj_t;

// The callback arg is a small-int or a ROM Pin object, so no need to scan by GC
STATIC mp_obj_t extint_callback_arg[NUM_VECTORS];
uint32_t extint_line_enable[NUM_VECTORS];

static void callback_handler(uint32_t line) {
    mp_obj_t *cb = &MP_STATE_PORT(extint_callback)[line];
    if (*cb != mp_const_none) {
        mp_sched_lock();
        // When executing code within a handler we must lock the GC to prevent
        // any memory allocations.  We must also catch any exceptions.
        gc_lock();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_call_function_1(*cb, extint_callback_arg[line]);
            nlr_pop();
        } else {
            // Uncaught exception; disable the callback so it doesn't run again.
            *cb = mp_const_none;
            extint_disable(line);
            printf("Uncaught exception in ExtInt interrupt handler line %u\n", (unsigned int)line);
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
        }
        gc_unlock();
        mp_sched_unlock();
    }
}

/**
 * These are the default interrupt handlers for the GPIO pins.  These functions
 * determine which pin originated the interrupt and call the callback file
 * associated with that pin.
 */
static void pint0_handler(uint32_t iid,
                          void *handlerArg) {
    int i;
    for (i = 0; i < 16; i++) {
        // Check to see if we have an interrupt pending and it's one we have a
        // call back for
        if ((*pREG_PINT0_REQ & (0x1 << i)) && extint_line_enable[0*16 + i] != 0) {
            *pREG_PINT0_REQ |= (0x1 << i);  // Clear interrupt
            callback_handler(0*16 + i);
        }
    }
}
static void pint1_handler(uint32_t iid,
                          void *handlerArg) {
    int i;
    for (i = 0; i < 16; i++) {
        // Check to see if we have an interrupt pending and it's one we have a
        // call back for
        if ((*pREG_PINT1_REQ & (0x1 << i)) && extint_line_enable[1*16 + i] != 0) {
            *pREG_PINT1_REQ |= (0x1 << i);  // Clear interrupt
            callback_handler(1*16 + i);
        }
    }
}
static void pint2_handler(uint32_t iid,
                          void *handlerArg) {
    int i;
    for (i = 0; i < 16; i++) {
        // Check to see if we have an interrupt pending and it's one we have a
        // call back for
        if ((*pREG_PINT2_REQ & (0x1 << i)) && extint_line_enable[2*16 + i] != 0) {
            *pREG_PINT2_REQ |= (0x1 << i);  // Clear interrupt
            callback_handler(2*16 + i);
        }
    }
}
static void pint3_handler(uint32_t iid,
                          void *handlerArg) {
    int i;
    for (i = 0; i < 16; i++) {
        // Check to see if we have an interrupt pending and it's one we have a
        // call back for
        if ((*pREG_PINT3_REQ & (0x1 << i)) && extint_line_enable[3*16 + i] != 0) {
            *pREG_PINT3_REQ |= (0x1 << i);  // Clear interrupt
            callback_handler(3*16 + i);
        }
    }
}
static void pint4_handler(uint32_t iid,
                          void *handlerArg) {
    int i;
    for (i = 0; i < 16; i++) {
        // Check to see if we have an interrupt pending and it's one we have a
        // call back for
        if ((*pREG_PINT4_REQ & (0x1 << i)) && extint_line_enable[4*16 + i] != 0) {
            *pREG_PINT4_REQ |= (0x1 << i);  // Clear interrupt
            callback_handler(4*16 + i);
        }
    }
}
static void pint5_handler(uint32_t iid,
                          void *handlerArg) {
    int i;
    for (i = 0; i < 16; i++) {
        // Check to see if we have an interrupt pending and it's one we have a
        // call back for
        if ((*pREG_PINT5_REQ & (0x1 << i)) && extint_line_enable[5*16 + i] != 0) {
            *pREG_PINT5_REQ |= (0x1 << i);  // Clear interrupt
            callback_handler(5*16 + i);
        }
    }
}

uint extint_reg_pin(const pin_obj_t *pin, uint32_t mode, uint32_t pull, mp_obj_t callback_obj, bool override_callback_obj) {
    ADI_GPIO_PORT port = (ADI_GPIO_PORT)(pin->port);
    uint16_t pinnumber = pin->pin;
    uint32_t pinmask = (uint32_t)(1 << pinnumber);
    uint32_t v_line = pin->port * 16 + pin->pin;

    if (mode != GPIO_MODE_IT_RISING &&
        mode != GPIO_MODE_IT_FALLING &&
        mode != GPIO_MODE_IT_LOW_LEVEL) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid ExtInt Mode: %d", mode));
    }
    // SC5XX doesn't support Pull Up/Down. There is no need to check.

    mp_obj_t *cb = &MP_STATE_PORT(extint_callback)[v_line];
    if (!override_callback_obj && *cb != mp_const_none && callback_obj != mp_const_none) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ExtInt vector %d is already in use", v_line));
    }

    extint_disable(v_line);
    *cb = callback_obj;

    volatile uint32_t *pREG_PORT_INEN_SET;
    volatile uint32_t *pREG_PORT_DIR_CLR;
    volatile uint32_t *pREG_PINT_INV_CLR;
    volatile uint32_t *pREG_PINT_INV_SET;
    volatile uint32_t *pREG_PINT_EDGE_SET;
    volatile uint32_t *pREG_PINT_EDGE_CLR;
    volatile uint32_t *pREG_PINT_ASSIGN;
    volatile uint32_t *pREG_PINT_MSK_SET;
    uint32_t INTR_PINT_BLOCK;
    void (*pint_handler)(uint32_t, void *);

    if (*cb != mp_const_none) {
        extint_callback_arg[v_line] = MP_OBJ_NEW_SMALL_INT(v_line);

        switch (port) {
            case ADI_GPIO_PORT_A:
                pREG_PORT_INEN_SET = pREG_PORTA_INEN_SET;
                pREG_PORT_DIR_CLR =  pREG_PORTA_DIR_CLR;
                pREG_PINT_INV_SET  = pREG_PINT0_INV_SET;
                pREG_PINT_INV_CLR  = pREG_PINT0_INV_CLR;
                pREG_PINT_EDGE_SET = pREG_PINT0_EDGE_SET;
                pREG_PINT_EDGE_CLR = pREG_PINT0_EDGE_CLR;
                pREG_PINT_ASSIGN   = pREG_PINT0_ASSIGN;
                pREG_PINT_MSK_SET  = pREG_PINT0_MSK_SET;
                INTR_PINT_BLOCK    = INTR_PINT0_BLOCK;
                pint_handler        = pint0_handler;
                break;

            case ADI_GPIO_PORT_B:
                pREG_PORT_INEN_SET = pREG_PORTB_INEN_SET;
                pREG_PORT_DIR_CLR =  pREG_PORTB_DIR_CLR;
                pREG_PINT_INV_SET  = pREG_PINT1_INV_SET;
                pREG_PINT_INV_CLR  = pREG_PINT1_INV_CLR;
                pREG_PINT_EDGE_SET = pREG_PINT1_EDGE_SET;
                pREG_PINT_EDGE_CLR = pREG_PINT1_EDGE_CLR;
                pREG_PINT_ASSIGN   = pREG_PINT1_ASSIGN;
                pREG_PINT_MSK_SET  = pREG_PINT1_MSK_SET;
                INTR_PINT_BLOCK    = INTR_PINT1_BLOCK;
                pint_handler        = pint1_handler;
                break;

            case ADI_GPIO_PORT_C:
                pREG_PORT_INEN_SET = pREG_PORTC_INEN_SET;
                pREG_PORT_DIR_CLR =  pREG_PORTC_DIR_CLR;
                pREG_PINT_INV_SET  = pREG_PINT2_INV_SET;
                pREG_PINT_INV_CLR  = pREG_PINT2_INV_CLR;
                pREG_PINT_EDGE_SET = pREG_PINT2_EDGE_SET;
                pREG_PINT_EDGE_CLR = pREG_PINT2_EDGE_CLR;
                pREG_PINT_ASSIGN   = pREG_PINT2_ASSIGN;
                pREG_PINT_MSK_SET  = pREG_PINT2_MSK_SET;
                INTR_PINT_BLOCK    = INTR_PINT2_BLOCK;
                pint_handler        = pint2_handler;
                break;

            case ADI_GPIO_PORT_D:
                pREG_PORT_INEN_SET = pREG_PORTD_INEN_SET;
                pREG_PORT_DIR_CLR =  pREG_PORTD_DIR_CLR;
                pREG_PINT_INV_SET  = pREG_PINT3_INV_SET;
                pREG_PINT_INV_CLR  = pREG_PINT3_INV_CLR;
                pREG_PINT_EDGE_SET = pREG_PINT3_EDGE_SET;
                pREG_PINT_EDGE_CLR = pREG_PINT3_EDGE_CLR;
                pREG_PINT_ASSIGN   = pREG_PINT3_ASSIGN;
                pREG_PINT_MSK_SET  = pREG_PINT3_MSK_SET;
                INTR_PINT_BLOCK    = INTR_PINT3_BLOCK;
                pint_handler        = pint3_handler;
                break;

            case ADI_GPIO_PORT_E:
                pREG_PORT_INEN_SET = pREG_PORTE_INEN_SET;
                pREG_PORT_DIR_CLR =  pREG_PORTE_DIR_CLR;
                pREG_PINT_INV_SET  = pREG_PINT4_INV_SET;
                pREG_PINT_INV_CLR  = pREG_PINT4_INV_CLR;
                pREG_PINT_EDGE_SET = pREG_PINT4_EDGE_SET;
                pREG_PINT_EDGE_CLR = pREG_PINT4_EDGE_CLR;
                pREG_PINT_ASSIGN   = pREG_PINT4_ASSIGN;
                pREG_PINT_MSK_SET  = pREG_PINT4_MSK_SET;
                INTR_PINT_BLOCK    = INTR_PINT4_BLOCK;
                pint_handler        = pint4_handler;
                break;

            case ADI_GPIO_PORT_F:
                pREG_PORT_INEN_SET = pREG_PORTF_INEN_SET;
                pREG_PORT_DIR_CLR =  pREG_PORTF_DIR_CLR;
                pREG_PINT_INV_SET  = pREG_PINT5_INV_SET;
                pREG_PINT_INV_CLR  = pREG_PINT5_INV_CLR;
                pREG_PINT_EDGE_SET = pREG_PINT5_EDGE_SET;
                pREG_PINT_EDGE_CLR = pREG_PINT5_EDGE_CLR;
                pREG_PINT_ASSIGN   = pREG_PINT5_ASSIGN;
                pREG_PINT_MSK_SET  = pREG_PINT5_MSK_SET;
                INTR_PINT_BLOCK    = INTR_PINT5_BLOCK;
                pint_handler        = pint5_handler;

                break;
            default:
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Invalid GPIO Port: %d", port));
                break;
        }

        *pREG_PORT_INEN_SET = pinmask;
        *pREG_PORT_DIR_CLR = pinmask;

        // Set up edge triggered interrupts for this port
        if (mode == GPIO_MODE_IT_RISING || mode == GPIO_MODE_IT_FALLING) {
            *pREG_PINT_EDGE_SET = pinmask;
            if      (mode == GPIO_MODE_IT_RISING) *pREG_PINT_INV_CLR = pinmask;
            else if (mode == GPIO_MODE_IT_FALLING) *pREG_PINT_INV_SET = pinmask;
        }
        // Set up level triggered interrupts for this port
        else if (mode == GPIO_MODE_IT_LOW_LEVEL) {
            *pREG_PINT_EDGE_CLR = pinmask;     // Clear edge interrupt
            *pREG_PINT_INV_SET = pinmask;      // Invert pin for active low
        }

        // Assign PINT to this port
        *pREG_PINT_ASSIGN = 0x0000;
        *pREG_PINT_MSK_SET = pinmask;

        adi_int_InstallHandler(INTR_PINT_BLOCK, pint_handler, NULL, true);

        extint_enable(v_line);
    }
    return v_line;
}

/// \moduleref machine
/// \class ExtInt - configure I/O pins to interrupt on external events
///
/// There are 6 interrupt lines dedicated for GPIOs, corresponding to PORTA - PORTF
///
/// Note: ExtInt will automatically configure the gpio line as an input.
///
///     extint = machine.ExtInt(pin, machine.ExtInt.IRQ_FALLING, machine.Pin.PULL_UP, callback)
///
/// Now every time a falling edge is seen on the X1 pin, the callback will be
/// called. Caution: mechanical pushbuttons have "bounce" and pushing or
/// releasing a switch will often generate multiple edges.
/// See: http://www.eng.utah.edu/~cs5780/debouncing.pdf for a detailed
/// explanation, along with various techniques for debouncing.
///
/// All pin objects go through the pin mapper to come up with one of the
/// gpio pins.
///
///     extint = machine.ExtInt(pin, mode, pull, callback)
///
/// Valid modes are machine.ExtInt.IRQ_RISING, machine.ExtInt.IRQ_FALLING,
/// machine.ExtInt.IRQ_RISING_FALLING, machine.ExtInt.EVT_RISING,
/// machine.ExtInt.EVT_FALLING, and machine.ExtInt.EVT_RISING_FALLING.
///
/// Only the IRQ_xxx modes have been tested. The EVT_xxx modes have
/// something to do with sleep mode and the WFE instruction.
///
/// Valid pull values are machine.Pin.PULL_UP, machine.Pin.PULL_DOWN, machine.Pin.PULL_NONE.
///
/// There is also a C API, so that drivers which require EXTI interrupt lines
/// can also use this code. See extint.h for the available functions and
/// usrsw.h for an example of using this.

// TODO Add python method to change callback object.

// Set override_callback_obj to true if you want to unconditionally set the
// callback function.
uint extint_register(mp_obj_t pin_obj, uint32_t mode, uint32_t pull, mp_obj_t callback_obj, bool override_callback_obj) {
    const pin_obj_t *pin = pin_find(pin_obj);
    return extint_reg_pin(pin, mode, pull, callback_obj, override_callback_obj);
}

// This function is intended to be used by the Pin.irq() method
void extint_register_pin(const pin_obj_t *pin, uint32_t mode, bool hard_irq, mp_obj_t callback_obj) {
    extint_reg_pin(pin, mode, 0, callback_obj, true);
}

void extint_enable(uint line) {
    extint_line_enable[line] = 1;
}

void extint_disable(uint line) {
    extint_line_enable[line] = 0;
}

/// \method line()
/// Return the line number that the pin is mapped to.
STATIC mp_obj_t extint_obj_line(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->line);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_line_obj, extint_obj_line);

/// \method enable()
/// Enable a disabled interrupt.
STATIC mp_obj_t extint_obj_enable(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    extint_enable(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_enable_obj, extint_obj_enable);

/// \method disable()
/// Disable the interrupt associated with the ExtInt object.
/// This could be useful for debouncing.
STATIC mp_obj_t extint_obj_disable(mp_obj_t self_in) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    extint_disable(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_disable_obj, extint_obj_disable);

/// \classmethod \constructor(pin, mode, pull, callback)
/// Create an ExtInt object:
///
///   - `pin` is the pin on which to enable the interrupt (can be a pin object or any valid pin name).
///   - `mode` can be one of:
///     - `ExtInt.IRQ_RISING` - trigger on a rising edge;
///     - `ExtInt.IRQ_FALLING` - trigger on a falling edge;
///     - `ExtInt.IRQ_LOW` - trigger when level is low.
///   - `pull` doesn't matter, since it is not supported. It is here for compatibility.
///   - `callback` is the function to call when the interrupt triggers.  The
///   callback function must accept exactly 1 argument, which is the line that
///   triggered the interrupt.
STATIC const mp_arg_t extint_make_new_args[] = {
    { MP_QSTR_pin,      MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_mode,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_pull,     MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};
#define EXTINT_MAKE_NEW_NUM_ARGS MP_ARRAY_SIZE(extint_make_new_args)

STATIC mp_obj_t extint_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // type_in == extint_obj_type

    // parse args
    mp_arg_val_t vals[EXTINT_MAKE_NEW_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, EXTINT_MAKE_NEW_NUM_ARGS, extint_make_new_args, vals);

    extint_obj_t *self = m_new_obj(extint_obj_t);
    self->base.type = type;
    self->line = extint_register(vals[0].u_obj, vals[1].u_int, vals[2].u_int, vals[3].u_obj, false);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void extint_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    extint_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ExtInt line=%u>", self->line);
}

STATIC const mp_rom_map_elem_t extint_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_line),    MP_ROM_PTR(&extint_obj_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),  MP_ROM_PTR(&extint_obj_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&extint_obj_disable_obj) },

    // class constants
    /// \constant IRQ_RISING - interrupt on a rising edge
    /// \constant IRQ_FALLING - interrupt on a falling edge
    /// \constant IRQ_LOW_LEVEL - interrupt on a low level
    { MP_ROM_QSTR(MP_QSTR_IRQ_RISING),         MP_ROM_INT(GPIO_MODE_IT_RISING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_FALLING),        MP_ROM_INT(GPIO_MODE_IT_FALLING) },
    { MP_ROM_QSTR(MP_QSTR_IRQ_LOW_LEVEL),      MP_ROM_INT(GPIO_MODE_IT_LOW_LEVEL) },
};

STATIC MP_DEFINE_CONST_DICT(extint_locals_dict, extint_locals_dict_table);

const mp_obj_type_t extint_type = {
    { &mp_type_type },
    .name = MP_QSTR_ExtInt,
    .print = extint_obj_print,
    .make_new = extint_make_new,
    .locals_dict = (mp_obj_dict_t*)&extint_locals_dict,
};
