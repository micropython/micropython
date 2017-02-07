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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "inc/hw_types.h"
#include "interrupt.h"
#include "pybsleep.h"
#include "mpexception.h"
#include "mperror.h"
#include "mpirq.h"


/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
const mp_arg_t mp_irq_init_args[] = {
    { MP_QSTR_trigger,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_priority,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} }, // the lowest priority
    { MP_QSTR_handler,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_wake,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
};

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
STATIC uint8_t mp_irq_priorities[] = { INT_PRIORITY_LVL_7, INT_PRIORITY_LVL_6, INT_PRIORITY_LVL_5, INT_PRIORITY_LVL_4,
                                       INT_PRIORITY_LVL_3, INT_PRIORITY_LVL_2, INT_PRIORITY_LVL_1 };

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void mp_irq_init0 (void) {
    // initialize the callback objects list
    mp_obj_list_init(&MP_STATE_PORT(mp_irq_obj_list), 0);
}

mp_obj_t mp_irq_new (mp_obj_t parent, mp_obj_t handler, const mp_irq_methods_t *methods) {
    mp_irq_obj_t *self = m_new_obj(mp_irq_obj_t);
    self->base.type = &mp_irq_type;
    self->handler = handler;
    self->parent = parent;
    self->methods = (mp_irq_methods_t *)methods;
    self->isenabled = true;
    // remove it in case it was already registered
    mp_irq_remove(parent);
    mp_obj_list_append(&MP_STATE_PORT(mp_irq_obj_list), self);
    return self;
}

mp_irq_obj_t *mp_irq_find (mp_obj_t parent) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mp_irq_obj_list).len; i++) {
        mp_irq_obj_t *callback_obj = ((mp_irq_obj_t *)(MP_STATE_PORT(mp_irq_obj_list).items[i]));
        if (callback_obj->parent == parent) {
            return callback_obj;
        }
    }
    return NULL;
}

void mp_irq_wake_all (void) {
    // re-enable all active callback objects one by one
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mp_irq_obj_list).len; i++) {
        mp_irq_obj_t *callback_obj = ((mp_irq_obj_t *)(MP_STATE_PORT(mp_irq_obj_list).items[i]));
        if (callback_obj->isenabled) {
            callback_obj->methods->enable(callback_obj->parent);
        }
    }
}

void mp_irq_disable_all (void) {
    // re-enable all active callback objects one by one
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mp_irq_obj_list).len; i++) {
        mp_irq_obj_t *callback_obj = ((mp_irq_obj_t *)(MP_STATE_PORT(mp_irq_obj_list).items[i]));
        callback_obj->methods->disable(callback_obj->parent);
    }
}

void mp_irq_remove (const mp_obj_t parent) {
    mp_irq_obj_t *callback_obj;
    if ((callback_obj = mp_irq_find(parent))) {
        mp_obj_list_remove(&MP_STATE_PORT(mp_irq_obj_list), callback_obj);
    }
}

uint mp_irq_translate_priority (uint priority) {
    if (priority < 1 || priority > MP_ARRAY_SIZE(mp_irq_priorities)) {
        mp_raise_ValueError(mpexception_value_invalid_arguments);
    }
    return mp_irq_priorities[priority - 1];
}

void mp_irq_handler (mp_obj_t self_in) {
    mp_irq_obj_t *self = self_in;
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

STATIC mp_obj_t mp_irq_init (mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_irq_obj_t *self = pos_args[0];
    // this is a bit of a hack, but it let us reuse the callback_create method from our parent
    ((mp_obj_t *)pos_args)[0] = self->parent;
    self->methods->init (n_args, pos_args, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_irq_init_obj, 1, mp_irq_init);

STATIC mp_obj_t mp_irq_enable (mp_obj_t self_in) {
    mp_irq_obj_t *self = self_in;
    self->methods->enable(self->parent);
    self->isenabled = true;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_irq_enable_obj, mp_irq_enable);

STATIC mp_obj_t mp_irq_disable (mp_obj_t self_in) {
    mp_irq_obj_t *self = self_in;
    self->methods->disable(self->parent);
    self->isenabled = false;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_irq_disable_obj, mp_irq_disable);

STATIC mp_obj_t mp_irq_flags (mp_obj_t self_in) {
    mp_irq_obj_t *self = self_in;
    return mp_obj_new_int(self->methods->flags(self->parent));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_irq_flags_obj, mp_irq_flags);

STATIC mp_obj_t mp_irq_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    mp_irq_handler (self_in);
    return mp_const_none;
}

STATIC const mp_map_elem_t mp_irq_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),                (mp_obj_t)&mp_irq_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable),              (mp_obj_t)&mp_irq_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable),             (mp_obj_t)&mp_irq_disable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flags),               (mp_obj_t)&mp_irq_flags_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_irq_locals_dict, mp_irq_locals_dict_table);

const mp_obj_type_t mp_irq_type = {
    { &mp_type_type },
    .name = MP_QSTR_irq,
    .call = mp_irq_call,
    .locals_dict = (mp_obj_t)&mp_irq_locals_dict,
};

