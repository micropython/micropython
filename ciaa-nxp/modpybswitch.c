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

typedef struct _pyb_switch_obj_t {
    mp_obj_base_t base;
    mp_obj_t callback;
} pyb_switch_obj_t;

STATIC pyb_switch_obj_t pyb_switch_obj[] = {
    {{&pyb_switch_type}},
    {{&pyb_switch_type}},
    {{&pyb_switch_type}},
    {{&pyb_switch_type}},
};

#define NUM_SWITCH MP_ARRAY_SIZE(pyb_switch_obj)
#define SWITCH_ID(obj) ((obj) - &pyb_switch_obj[0] + 1)

void pyb_switch_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_switch_obj_t *self = self_in;
    mp_printf(print, "Switch(%u)", SWITCH_ID(self));
}

STATIC mp_obj_t pyb_switch_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t sw_id = mp_obj_get_int(args[0]);
    if (!(1 <= sw_id && sw_id <= NUM_SWITCH)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Switch %d does not exist", sw_id));
    }
    return (mp_obj_t)&pyb_switch_obj[sw_id - 1];
}

mp_obj_t pyb_switch_value(mp_obj_t self_in) {
    pyb_switch_obj_t *self = self_in;
    //return switch_get(SWITCH_ID(self)) ? mp_const_true : mp_const_false;
    return Buttons_GetStatusByNumber(SWITCH_ID(self)-1) ? mp_const_true : mp_const_false;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_switch_value_obj, pyb_switch_value);


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


mp_obj_t pyb_switch_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return pyb_switch_value(self_in);
}

STATIC const mp_map_elem_t pyb_switch_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_switch), (mp_obj_t)&pyb_switch_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&pyb_switch_callback_obj },
};

STATIC MP_DEFINE_CONST_DICT(pyb_switch_locals_dict, pyb_switch_locals_dict_table);

const mp_obj_type_t pyb_switch_type = {
    { &mp_type_type },
    .name = MP_QSTR_Switch,
    .print = pyb_switch_print,
    .make_new = pyb_switch_make_new,
    .call = pyb_switch_call,
    .locals_dict = (mp_obj_t)&pyb_switch_locals_dict,
};


