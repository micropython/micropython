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
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/gc.h"


#include "osapi.h"
#include "os_type.h"

#include "gccollect.h"

#include "timer.h"
#include "utils.h"

extern void ets_timer_disarm(ETSTimer *ptimer);
extern void ets_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg);
extern void ets_timer_arm_new(ETSTimer *ptimer,uint32_t milliseconds, bool repeat_flag, bool);

typedef struct _esp_timer_obj_t {
    mp_obj_base_t base;
    os_timer_t timer;
    mp_obj_t callback;
    uint32_t period;    // in milliseconds
    bool repeat;
    mp_obj_t arg;
    // start_time; // to calculate elapsed, time to fire etc
} esp_timer_obj_t;


STATIC void esp_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    esp_timer_obj_t *self = self_in;

    mp_printf(print, "timer(period=%u, callback=%x, repeat=%c)", 
        self->period,
        (unsigned int)&self->callback,
        self->repeat ? 'y' : 'n');
}

STATIC void timer_common_callback(void *parg)
{
    esp_timer_obj_t *self = (esp_timer_obj_t *)parg;
    if (self->callback) 
        call_function_1_protected(self->callback, self->arg);
    gc_collect();       // done out timed job and are going back to the operating command
}

STATIC const mp_arg_t esp_timer_init_args[] = {
    { MP_QSTR_arg,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_repeat,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_bool = true} }
};
#define PYB_TIMER_INIT_NUM_ARGS MP_ARRAY_SIZE(esp_timer_init_args)

STATIC mp_obj_t esp_timer_init_helper(esp_timer_obj_t *self, uint n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_val_t vals[PYB_TIMER_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, PYB_TIMER_INIT_NUM_ARGS, esp_timer_init_args, vals);
    self->arg = vals[0].u_obj;
    self->repeat = vals[1].u_bool;
    return self;
}


STATIC mp_obj_t esp_timer_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 2, MP_OBJ_FUN_ARGS_MAX, true);
    
    esp_timer_obj_t *self = m_new_obj(esp_timer_obj_t);
    memset(self, 0, sizeof(*self));

    self->base.type = &esp_timer_type;
    self->callback = args[0];
    self->period = mp_obj_get_int(args[1]);
    self->arg = mp_const_none;  // init these here as the kw arg processis is only called for more than 2 args
    self->repeat = true;
    if (self->period == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "can't have 0 period"));
    }
    if (n_args > 2 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        self = esp_timer_init_helper(self, n_args - 2, args + 2, &kw_args);
    }
    os_timer_setfn(&self->timer, timer_common_callback, self);
    os_timer_arm(&self->timer, self->period, self->repeat);

    return (mp_obj_t)self;
}

STATIC mp_obj_t esp_timer_cancel(mp_obj_t self_in) {
    esp_timer_obj_t *self = self_in;
    ets_timer_disarm(&self->timer);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_timer_cancel_obj, esp_timer_cancel);


STATIC const mp_map_elem_t esp_timer_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_cancel), (mp_obj_t)&esp_timer_cancel_obj },
};
STATIC MP_DEFINE_CONST_DICT(esp_timer_locals_dict, esp_timer_locals_dict_table);

const mp_obj_type_t esp_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_timer,
    .print = esp_timer_print,
    .make_new = esp_timer_make_new,
    .locals_dict = (mp_obj_t)&esp_timer_locals_dict,
};

