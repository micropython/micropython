/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "wm_include.h"
#include "wm_timer.h"

#include "py/obj.h"
#include "py/runtime.h"
//#include "modmachine.h"
//#include "mphalport.h"

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    u8 timerid;
    u32 timeout;
    bool is_repeat;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

STATIC void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = self_in;

    mp_printf(print, "Timer(%p; ", self);

    mp_printf(print, "timerid=%d, ", self->timerid);
    mp_printf(print, "period=%d, ", self->timeout);
    mp_printf(print, "auto_reload=%d)", self->is_repeat);
}

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    machine_timer_obj_t *self = m_new_obj(machine_timer_obj_t);
    self->base.type = &machine_timer_type;
    self->timerid = WM_TIMER_ID_INVALID;
    self->timeout = 100;
    self->is_repeat = TRUE;
    return self;
}

STATIC mp_obj_t machine_timer_del(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    if (WM_TIMER_ID_INVALID != self->timerid) {
        tls_timer_destroy(self->timerid);
        self->timerid = WM_TIMER_ID_INVALID;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_del_obj, machine_timer_del);

STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    if (WM_TIMER_ID_INVALID != self->timerid)
        tls_timer_stop(self->timerid);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC mp_obj_t machine_timer_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = (machine_timer_obj_t *)args[0];
    struct tls_timer_cfg timercfg;
    memset(&timercfg, 0, sizeof(timercfg));
    timercfg.is_repeat = TRUE;
    timercfg.unit = TLS_TIMER_UNIT_MS;

    enum {
        ARG_mode,
        ARG_period,
        ARG_callback,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_period,       MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_callback,     MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mp_arg_val_t dargs[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, dargs);

    if (2 == n_args) {
        timercfg.timeout = dargs[0].u_int;
    } else if (3 == n_args) {
        timercfg.is_repeat = dargs[ARG_mode].u_int;
        timercfg.timeout = dargs[ARG_period].u_int;
    } else if (4 == n_args) {
        timercfg.is_repeat = dargs[ARG_mode].u_int;
        timercfg.timeout = dargs[ARG_period].u_int;
        timercfg.callback = dargs[ARG_callback].u_obj;
    } else {
        mp_raise_ValueError("invalid format");
    }

    if (WM_TIMER_ID_INVALID != self->timerid) {
        tls_timer_stop(self->timerid);
    } else {
        self->timerid = tls_timer_create(&timercfg);
    }
    tls_timer_start(self->timerid);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(false) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(true) },
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = machine_timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_t) &machine_timer_locals_dict,
};

