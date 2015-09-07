/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "std.h"

#include "py/mpconfig.h"
#include MICROPY_HAL_H
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "inc/hw_types.h"
#include "interrupt.h"
#include "pybsleep.h"
#include "mpcallback.h"
#include "mpexception.h"
#include "mperror.h"


/******************************************************************************
 DEFINE PUBLIC DATA
 ******************************************************************************/
const mp_arg_t mpcallback_init_args[] = {
    { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_handler,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_priority,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
    { MP_QSTR_value,        MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_wake_from,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PYB_PWR_MODE_ACTIVE} },
};

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void mpcallback_init0 (void) {
    // initialize the callback objects list
    mp_obj_list_init(&MP_STATE_PORT(mpcallback_obj_list), 0);
}

mp_obj_t mpcallback_new (mp_obj_t parent, mp_obj_t handler, const mp_cb_methods_t *methods, bool enable) {
    mpcallback_obj_t *self = m_new_obj(mpcallback_obj_t);
    self->base.type = &pyb_callback_type;
    self->handler = handler;
    self->parent = parent;
    self->methods = (mp_cb_methods_t *)methods;
    self->isenabled = enable;
    // remove it in case it was already registered
    mpcallback_remove(parent);
    mp_obj_list_append(&MP_STATE_PORT(mpcallback_obj_list), self);
    return self;
}

mpcallback_obj_t *mpcallback_find (mp_obj_t parent) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mpcallback_obj_list).len; i++) {
        mpcallback_obj_t *callback_obj = ((mpcallback_obj_t *)(MP_STATE_PORT(mpcallback_obj_list).items[i]));
        if (callback_obj->parent == parent) {
            return callback_obj;
        }
    }
    return NULL;
}

void mpcallback_wake_all (void) {
    // re-enable all active callback objects one by one
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mpcallback_obj_list).len; i++) {
        mpcallback_obj_t *callback_obj = ((mpcallback_obj_t *)(MP_STATE_PORT(mpcallback_obj_list).items[i]));
        if (callback_obj->isenabled) {
            callback_obj->methods->enable(callback_obj->parent);
        }
    }
}

void mpcallback_disable_all (void) {
    // re-enable all active callback objects one by one
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mpcallback_obj_list).len; i++) {
        mpcallback_obj_t *callback_obj = ((mpcallback_obj_t *)(MP_STATE_PORT(mpcallback_obj_list).items[i]));
        callback_obj->methods->disable(callback_obj->parent);
    }
}

void mpcallback_remove (const mp_obj_t parent) {
    mpcallback_obj_t *callback_obj;
    if ((callback_obj = mpcallback_find(parent))) {
        mp_obj_list_remove(&MP_STATE_PORT(mpcallback_obj_list), callback_obj);
    }
}

uint mpcallback_translate_priority (uint priority) {
    if (priority < 1 || priority > 7) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }

   switch (priority) {
   case 1:
       return INT_PRIORITY_LVL_7;
   case 2:
       return INT_PRIORITY_LVL_6;
   case 3:
       return INT_PRIORITY_LVL_5;
   case 4:
       return INT_PRIORITY_LVL_4;
   case 5:
       return INT_PRIORITY_LVL_3;
   case 6:
       return INT_PRIORITY_LVL_2;
   case 7:
       return INT_PRIORITY_LVL_1;
   default:
       return INT_PRIORITY_LVL_7;
   }
}

void mpcallback_handler (mp_obj_t self_in) {
    mpcallback_obj_t *self = self_in;
    if (self && self->handler != mp_const_none) {
        // when executing code within a handler we must lock the GC to prevent
        // any memory allocations.
        gc_lock();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_call_function_1(self->handler, self->parent);
            nlr_pop();
        }
        else {
            // uncaught exception; disable the callback so that it doesn't run again
            self->methods->disable (self->parent);
            self->handler = mp_const_none;
            // signal the error using the heart beat led and
            // by printing a message
            printf("Uncaught exception in callback handler\n");
            mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
            mperror_signal_error();
        }
        gc_unlock();
    }
}

/******************************************************************************/
// Micro Python bindings

/// \method init()
/// Initializes the interrupt callback. With no parameters passed, everything will default
/// to the values assigned to mpcallback_init_args[].
STATIC mp_obj_t callback_init(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mpcallback_obj_t *self = pos_args[0];
    // this is a bit of a hack, but it let us reuse the callback_create method from our parent
    ((mp_obj_t *)pos_args)[0] = self->parent;
    self->methods->init (n_args, pos_args, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(callback_init_obj, 1, callback_init);

/// \method enable()
/// Enables the interrupt callback
STATIC mp_obj_t callback_enable (mp_obj_t self_in) {
    mpcallback_obj_t *self = self_in;
    self->methods->enable(self->parent);
    self->isenabled = true;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(callback_enable_obj, callback_enable);

/// \method disable()
/// Disables the interrupt callback
STATIC mp_obj_t callback_disable (mp_obj_t self_in) {
    mpcallback_obj_t *self = self_in;
    self->methods->disable(self->parent);
    self->isenabled = false;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(callback_disable_obj, callback_disable);

/// \method \call()
/// Triggers the interrupt callback
STATIC mp_obj_t callback_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    mpcallback_handler (self_in);
    return mp_const_none;
}

STATIC const mp_map_elem_t callback_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                (mp_obj_t)&callback_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable),              (mp_obj_t)&callback_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable),             (mp_obj_t)&callback_disable_obj },
};

STATIC MP_DEFINE_CONST_DICT(callback_locals_dict, callback_locals_dict_table);

const mp_obj_type_t pyb_callback_type = {
    { &mp_type_type },
    .name = MP_QSTR_callback,
    .call = callback_call,
    .locals_dict = (mp_obj_t)&callback_locals_dict,
};

