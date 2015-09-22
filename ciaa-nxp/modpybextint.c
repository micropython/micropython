/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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

#include "py/runtime.h"
#include "py/nlr.h"
#include "py/gc.h"

#include "board.h"
#include "modpyb.h"
#include "modpybpin.h"
#include "ciaanxp_mphal.h"

typedef struct {
    mp_obj_base_t base;
    mp_int_t line;
    mp_obj_t callback;
    uint8_t flagEdgeLevel;
    uint8_t flagHighLow;
} extint_obj_t;

void extint_exec_callback(extint_obj_t* self)
{
            // execute callback if it's set
            if (self->callback != mp_const_none) {
                // When executing code within a handler we must lock the GC to prevent
                // any memory allocations.  We must also catch any exceptions.
                gc_lock();
                nlr_buf_t nlr;
                if (nlr_push(&nlr) == 0) {
                    mp_call_function_1(self->callback, MP_OBJ_NEW_SMALL_INT(self->line));
                    nlr_pop();
                } else {
                    // Uncaught exception; disable the callback so it doesn't run again.
                    self->callback = mp_const_none;
                    printf("uncaught exception in ExtInt line (%d) interrupt handler\n",(int)self->line);
                    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
                }
                gc_unlock();
            }
}

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

    extint_obj_t *self = m_new_obj(extint_obj_t);
    self->base.type = type_in;
    //self->line = extint_register(vals[0].u_obj, vals[1].u_int, vals[2].u_int, vals[3].u_obj, false);
    if (MP_OBJ_IS_INT(vals[0].u_obj))
	self->line = mp_obj_get_int(vals[0].u_obj);
    else
	self->line = getPinNumber(vals[0].u_obj);

    if(self->line > 8)
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "ExtInt vector %d > 8", self->line));

    // get pull mode
    uint pull = vals[2].u_int;
    if (pull!=GPIO_NOPULL && pull!=GPIO_PULLUP && pull!=GPIO_PULLDOWN) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid pin pull: %d", pull));
    }

    // get interrupt mode
    self->flagEdgeLevel = 1; // always edge for now
    if(vals[1].u_int!=GPIO_MODE_IT_RISING && vals[1].u_int!=GPIO_MODE_IT_FALLING && vals[1].u_int!=GPIO_MODE_IT_RISING_FALLING)
	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid mode: %d", vals[1].u_int));
    if(vals[1].u_int==GPIO_MODE_IT_RISING_FALLING)
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "mode not supported"));
    if(vals[1].u_int==GPIO_MODE_IT_RISING)
        self->flagHighLow = 1;
    else
	self->flagHighLow = 0;

    // check callback value
    mp_obj_t callback = vals[3].u_obj;
    if (callback == mp_const_none) {
        // stop interrupt
        self->callback = mp_const_none;
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "callback must be None or a callable object"));
    }
    //_____________________


    // configure GPIO as input and enable Interrupt if it is available
    if(self->callback!= mp_const_none)
    {
        mp_hal_configureGPIOs(self->line,GPIO_MODE_INPUT,pull);
    	if(mp_hal_enableIntCallbackGPIO(self->line,(void (*)(void *))extint_exec_callback,self,self->flagEdgeLevel,self->flagHighLow)!=1)
		nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Interrupt line not available"));
    }
    else
	mp_hal_disableIntCallbackGPIO(self->line);
    return self;
}


STATIC void extint_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    extint_obj_t *self = self_in;
    mp_printf(print, "<ExtInt line=%u>", self->line);
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
    if(self->callback!= mp_const_none)
    {

    	if(mp_hal_enableIntCallbackGPIO(self->line,(void (*)(void *))extint_exec_callback,self,self->flagEdgeLevel,self->flagHighLow)!=1)
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Interrupt line not available"));
   }
   else
	nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "Invalid callback function"));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_enable_obj, extint_obj_enable);



/// \method disable()
/// Disable the interrupt associated with the ExtInt object.
/// This could be useful for debouncing.
STATIC mp_obj_t extint_obj_disable(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    mp_hal_disableIntCallbackGPIO(self->line);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_disable_obj, extint_obj_disable);

/// \method swint()
/// Trigger the callback from software.
STATIC mp_obj_t extint_obj_swint(mp_obj_t self_in) {
    extint_obj_t *self = self_in;
    extint_exec_callback(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(extint_obj_swint_obj,  extint_obj_swint);


STATIC const mp_map_elem_t pyb_extint_locals_dict_table[] = {

    { MP_OBJ_NEW_QSTR(MP_QSTR_line),    (mp_obj_t)&extint_obj_line_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable),  (mp_obj_t)&extint_obj_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable), (mp_obj_t)&extint_obj_disable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_swint),   (mp_obj_t)&extint_obj_swint_obj },

    // class constants
    /// \constant IRQ_RISING - interrupt on a rising edge
    /// \constant IRQ_FALLING - interrupt on a falling edge
    /// \constant IRQ_RISING_FALLING - interrupt on a rising or falling edge
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_RISING),         MP_OBJ_NEW_SMALL_INT(GPIO_MODE_IT_RISING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_FALLING),        MP_OBJ_NEW_SMALL_INT(GPIO_MODE_IT_FALLING) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IRQ_RISING_FALLING), MP_OBJ_NEW_SMALL_INT(GPIO_MODE_IT_RISING_FALLING) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_extint_locals_dict, pyb_extint_locals_dict_table);

const mp_obj_type_t pyb_extint_type = {
    { &mp_type_type },
    .name = MP_QSTR_ExtInt,
    .print = extint_obj_print,
    .make_new = extint_make_new,
    .locals_dict = (mp_obj_t)&pyb_extint_locals_dict,
};


