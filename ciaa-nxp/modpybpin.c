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
#include "ciaanxp_mphal.h"

typedef struct _pyb_gpio_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
} pyb_gpio_obj_t;

STATIC pyb_gpio_obj_t pyb_gpio_obj[] = {
    {{&pyb_gpio_type}},
    {{&pyb_gpio_type}},
    {{&pyb_gpio_type}},
    {{&pyb_gpio_type}},
    {{&pyb_gpio_type}},
    {{&pyb_gpio_type}},
    {{&pyb_gpio_type}},
    {{&pyb_gpio_type}},
    {{&pyb_gpio_type}},
};

#define GPIO_ID(obj) ((obj) - &pyb_gpio_obj[0])
#define NUM_GPIO 8

void pyb_gpio_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_gpio_obj_t *self = self_in;
    mp_printf(print, "GPIO(%u)", GPIO_ID(self));
}

STATIC mp_obj_t pyb_gpio_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t sw_id = mp_obj_get_int(args[0]);
    if (!(0 <= sw_id && sw_id <= NUM_GPIO)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "GPIO %d does not exist", sw_id));
    }
    return (mp_obj_t)&pyb_gpio_obj[sw_id];
}

/*
mp_obj_t pyb_switch_value(mp_obj_t self_in) {
    pyb_switch_obj_t *self = self_in;
    //return switch_get(SWITCH_ID(self)) ? mp_const_true : mp_const_false;
    return Buttons_GetStatusByNumber(SWITCH_ID(self)-1) ? mp_const_true : mp_const_false;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_switch_value_obj, pyb_switch_value);
*/

/*
void pyb_switch_exec_callback(pyb_switch_obj_t* self)
{
	    // execute callback if it's set
            if (self->callback != mp_const_none) {
                // When executing code within a handler we must lock the GC to prevent
                // any memory allocations.  We must also catch any exceptions.
                gc_lock();
                nlr_buf_t nlr;
                if (nlr_push(&nlr) == 0) {
                    mp_call_function_1(self->callback, self);
                    nlr_pop();
                } else {
                    // Uncaught exception; disable the callback so it doesn't run again.
                    self->callback = mp_const_none;
                    printf("uncaught exception in switch(%u) interrupt handler\n",SWITCH_ID(self));
                    mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
                }
                gc_unlock();
            }
}

/// \method callback(fun)
/// Set the function to be called when the switch changes.
/// `fun` is passed 1 argument, the switch object.
/// If `fun` is `None` then the callback will be disabled.
STATIC mp_obj_t pyb_switch_callback(mp_obj_t self_in, mp_obj_t callback) {
    pyb_switch_obj_t *self = self_in;
    if (callback == mp_const_none) {
        // stop interrupt
        self->callback = mp_const_none;
	mp_hal_configureButtonCallback(SWITCH_ID(self)-1,NULL,NULL);
    } else if (mp_obj_is_callable(callback)) {
        self->callback = callback;
	mp_hal_configureButtonCallback(SWITCH_ID(self)-1,( void(*)(void*)  )pyb_switch_exec_callback,self);
    } else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "callback must be None or a callable object"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_switch_callback_obj, pyb_switch_callback);

*/

/*
mp_obj_t pyb_gpio_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return pyb_switch_value(self_in);
}
*/

/// \method init(mode, pull=Pin.PULL_NONE, af=-1)
/// Initialise the pin:
///
///   - `mode` can be one of:
///     - `Pin.IN` - configure the pin for input;
///     - `Pin.OUT_PP` - configure the pin for output, with push-pull control;
///     - `Pin.OUT_OD` - configure the pin for output, with open-drain control;
///   - `pull` can be one of:
///     - `Pin.PULL_NONE` - no pull up or down resistors;
///     - `Pin.PULL_UP` - enable the pull-up resistor;
///     - `Pin.PULL_DOWN` - enable the pull-down resistor.
///
/// Returns: `None`.
STATIC const mp_arg_t pin_init_args[] = {
    { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
    { MP_QSTR_pull,                   MP_ARG_INT, {.u_int = GPIO_NOPULL}},
    { MP_QSTR_af,                     MP_ARG_INT, {.u_int = -1}},
};
#define PIN_INIT_NUM_ARGS MP_ARRAY_SIZE(pin_init_args)

STATIC mp_obj_t pin_obj_init_helper(const pyb_gpio_obj_t *self, mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PIN_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PIN_INIT_NUM_ARGS, pin_init_args, vals);

    // get io mode
    uint mode = vals[0].u_int;

    if (mode!=GPIO_MODE_INPUT && mode!=GPIO_MODE_OUTPUT_PP && mode!=GPIO_MODE_OUTPUT_OD) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid pin mode: %d", mode));
    }
    if(mode==GPIO_MODE_OUTPUT_OD) {
 	nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Open Drain mode not supported"));
    }

    // get pull mode
    uint pull = vals[1].u_int;

    if (pull!=GPIO_NOPULL && pull!=GPIO_PULLUP && pull!=GPIO_PULLDOWN) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid pin pull: %d", pull));
    }

    // configure the GPIO as requested
    mp_hal_configureGPIOs(GPIO_ID(self),mode,pull);

    return mp_const_none;
}

STATIC mp_obj_t pin_obj_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pin_init_obj, 1, pin_obj_init);


/// \method value([value])
/// Get or set the digital logic level of the pin:
///
///   - With no argument, return 0 or 1 depending on the logic level of the pin.
///   - With `value` given, set the logic level of the pin.  `value` can be
///   anything that converts to a boolean.  If it converts to `True`, the pin
///   is set high, otherwise it is set low.
STATIC mp_obj_t pin_value(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_gpio_obj_t *self = args[0];
    if (n_args == 1) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(mp_hal_readGPIO(GPIO_ID(self)));
    } else {
        // set pin
        if (mp_obj_is_true(args[1])) {
	    mp_hal_writeGPIO(GPIO_ID(self),1);
        } else {
            mp_hal_writeGPIO(GPIO_ID(self),0);
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_value_obj, 1, 2, pin_value);

/// \method low()
/// Set the pin to a low logic level.
STATIC mp_obj_t pin_low(mp_obj_t self_in) {
    pyb_gpio_obj_t *self = self_in;
    mp_hal_writeGPIO(GPIO_ID(self),0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_low_obj, pin_low);

/// \method high()
/// Set the pin to a high logic level.
STATIC mp_obj_t pin_high(mp_obj_t self_in) {
    pyb_gpio_obj_t *self = self_in;
    mp_hal_writeGPIO(GPIO_ID(self),1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_high_obj, pin_high);


STATIC const mp_map_elem_t pyb_gpio_locals_dict_table[] = {
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_gpio_callback_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_init),    (mp_obj_t)&pin_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value),   (mp_obj_t)&pin_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_low),     (mp_obj_t)&pin_low_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_high),    (mp_obj_t)&pin_high_obj },

    // class constants
    /// \constant IN - initialise the pin to input mode
    /// \constant OUT_PP - initialise the pin to output mode with a push-pull drive
    /// \constant OUT_OD - initialise the pin to output mode with an open-drain drive
    /// \constant PULL_NONE - don't enable any pull up or down resistors on the pin
    /// \constant PULL_UP - enable the pull-up resistor on the pin
    /// \constant PULL_DOWN - enable the pull-down resistor on the pin
    { MP_OBJ_NEW_QSTR(MP_QSTR_IN),        MP_OBJ_NEW_SMALL_INT(GPIO_MODE_INPUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT_PP),    MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OUTPUT_PP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT_OD),    MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OUTPUT_OD) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_NONE), MP_OBJ_NEW_SMALL_INT(GPIO_NOPULL) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_UP),   MP_OBJ_NEW_SMALL_INT(GPIO_PULLUP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_DOWN), MP_OBJ_NEW_SMALL_INT(GPIO_PULLDOWN) },

};

STATIC MP_DEFINE_CONST_DICT(pyb_gpio_locals_dict, pyb_gpio_locals_dict_table);

const mp_obj_type_t pyb_gpio_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pyb_gpio_print,
    .make_new = pyb_gpio_make_new,
    //.call = pyb_gpio_call,
    .locals_dict = (mp_obj_t)&pyb_gpio_locals_dict,
};


uint32_t getPinNumber(void* pObj)
{
    pyb_gpio_obj_t* self = (pyb_gpio_obj_t*) pObj;
    return GPIO_ID(self);
}
