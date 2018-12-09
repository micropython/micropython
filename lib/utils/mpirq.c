/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
 *               2018 Tobias Badertscher
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

#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "mpirq.h"


/******************************************************************************
 DECLARE PUBLIC DATA
 ******************************************************************************/
const mp_arg_t mp_irq_init_args[] = {
    { MP_QSTR_handler,      MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_trigger,      MP_ARG_INT, {.u_obj = mp_const_none} },
    { MP_QSTR_hard,         MP_ARG_BOOL,{.u_bool = false} },
};
/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/
void mp_irq_init0 (void) {
    // initialize the callback objects list
    mp_obj_list_init(&MP_STATE_PORT(mp_irq_obj_list), 0);
}

mp_obj_t mp_irq_new (mp_obj_t parent, mp_obj_t handler, bool hard, const mp_irq_methods_t *methods) {
    mp_irq_obj_t *self = mp_irq_find (parent);
    if (self==mp_const_none) {
        /*
         * We only add an IRQ object to an UART but never delete it
         * to avoid fragmentation of the heap.
         */
        self = m_new0(mp_irq_obj_t, 1);
        self->base.type = &mp_irq_type;
        self->parent = parent;
        self->methods = (mp_irq_methods_t *)methods;
        mp_obj_list_append(&MP_STATE_PORT(mp_irq_obj_list), self);
    }
    self->handler = handler;
    self->ishard = hard;
    return self;
}

mp_irq_obj_t *mp_irq_find (mp_obj_t parent) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mp_irq_obj_list).len; i++) {
        mp_irq_obj_t *callback_obj = ((mp_irq_obj_t *)(MP_STATE_PORT(mp_irq_obj_list).items[i]));
        if (callback_obj->parent == parent) {
            return callback_obj;
        }
    }
    return mp_const_none;
}

void mp_irq_deactivate_all (void) {
    // deactivate all active callback objects one by one
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mp_irq_obj_list).len; i++) {
        mp_irq_obj_t *callback_obj = ((mp_irq_obj_t *)(MP_STATE_PORT(mp_irq_obj_list).items[i]));
        callback_obj->methods->trigger(callback_obj->parent, 0);
    }
}

void mp_irq_handler (mp_obj_t self_in) {
    mp_irq_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if ((self != mp_const_none) && (self->handler != mp_const_none)) {
        if (self->ishard) {
            // when executing code within a handler we must lock the GC to prevent
            // any memory allocations.
            gc_lock();
            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0) {
                mp_call_function_1(self->handler, self->parent);
                nlr_pop();
            } else {
                // uncaught exception; disable the callback so that it doesn't run again
                self->methods->trigger(self->parent, 0);
                self->handler = mp_const_none;
                printf("Uncaught exception in IRQ callback handler\n");
                mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
            }
            gc_unlock();
        } else {
            // Schedule call to MP function
            mp_sched_schedule(self->handler, self->parent);
        }
    }
}

/******************************************************************************/
// MicroPython bindings

STATIC mp_obj_t mp_irq_flags (mp_obj_t self_in) {
    mp_irq_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->methods->info(self->parent, IRQ_INFO_FLAGS));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_irq_flags_obj, mp_irq_flags);

STATIC mp_obj_t mp_irq_trigger (size_t n_args, const mp_obj_t *args) {
    mp_irq_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t ret_obj = mp_obj_new_int(self->methods->info(self->parent, IRQ_INFO_TRIGGERS));
    if (n_args == 2) {
        // set trigger
        self->methods->trigger(self->parent, mp_obj_get_int(args[1]));
    }
    return ret_obj;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_irq_trigger_obj, 1, 2, mp_irq_trigger);

STATIC mp_obj_t mp_irq_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    mp_irq_handler (self_in);
    return mp_const_none;
}

STATIC const mp_rom_map_elem_t mp_irq_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_flags),               MP_ROM_PTR(&mp_irq_flags_obj) },
    { MP_ROM_QSTR(MP_QSTR_trigger),             MP_ROM_PTR(&mp_irq_trigger_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_irq_locals_dict, mp_irq_locals_dict_table);

const mp_obj_type_t mp_irq_type = {
    { &mp_type_type },
    .name = MP_QSTR_irq,
    .call = mp_irq_call,
    .locals_dict = (mp_obj_t)&mp_irq_locals_dict,
};

