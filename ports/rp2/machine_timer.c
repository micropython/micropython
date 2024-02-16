/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "py/mperrno.h"
#include "py/mphal.h"
#include "pico/time.h"

#define ALARM_ID_INVALID (-1)
#define TIMER_MODE_ONE_SHOT (0)
#define TIMER_MODE_PERIODIC (1)

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    struct alarm_pool *pool;
    alarm_id_t alarm_id;
    uint32_t mode;
    uint64_t delta_us; // for periodic mode
    mp_obj_t callback;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

STATIC int64_t alarm_callback(alarm_id_t id, void *user_data) {
    machine_timer_obj_t *self = user_data;
    mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(self));
    if (self->mode == TIMER_MODE_ONE_SHOT) {
        return 0;
    } else {
        return -self->delta_us;
    }
}

STATIC void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    qstr mode = self->mode == TIMER_MODE_ONE_SHOT ? MP_QSTR_ONE_SHOT : MP_QSTR_PERIODIC;
    mp_printf(print, "Timer(mode=%q, tick_hz=1000000, period=", mode);
    if (self->delta_us <= 0xffffffff) {
        mp_printf(print, "%u)", (uint32_t)self->delta_us);
    } else {
        mp_printf(print, "%u000)", (uint32_t)(self->delta_us / 1000));
    }
}

STATIC mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_mode, ARG_callback, ARG_period, ARG_tick_hz, ARG_freq, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_PERIODIC} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        { MP_QSTR_tick_hz,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->mode = args[ARG_mode].u_int;
    if (args[ARG_freq].u_obj != mp_const_none) {
        // Frequency specified in Hz
        #if MICROPY_PY_BUILTINS_FLOAT
        self->delta_us = (uint64_t)(MICROPY_FLOAT_CONST(1000000.0) / mp_obj_get_float(args[ARG_freq].u_obj));
        #else
        self->delta_us = 1000000 / mp_obj_get_int(args[ARG_freq].u_obj);
        #endif
    } else {
        // Period specified
        self->delta_us = (uint64_t)args[ARG_period].u_int * 1000000 / args[ARG_tick_hz].u_int;
    }
    if (self->delta_us < 1) {
        self->delta_us = 1;
    }

    self->callback = args[ARG_callback].u_obj;
    self->alarm_id = alarm_pool_add_alarm_in_us(self->pool, self->delta_us, alarm_callback, self, true);
    if (self->alarm_id == -1) {
        mp_raise_OSError(MP_ENOMEM);
    }

    return mp_const_none;
}

STATIC mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    machine_timer_obj_t *self = mp_obj_malloc_with_finaliser(machine_timer_obj_t, &machine_timer_type);
    self->pool = alarm_pool_get_default();
    self->alarm_id = ALARM_ID_INVALID;

    // Get timer id (only soft timer (-1) supported at the moment)
    mp_int_t id = -1;
    if (n_args > 0) {
        id = mp_obj_get_int(args[0]);
        --n_args;
        ++args;
    }
    if (id != -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("Timer doesn't exist"));
    }

    if (n_args > 0 || n_kw > 0) {
        // Start the timer
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (self->alarm_id != ALARM_ID_INVALID) {
        alarm_pool_cancel_alarm(self->pool, self->alarm_id);
        self->alarm_id = ALARM_ID_INVALID;
    }
    return machine_timer_init_helper(self, n_args - 1, args + 1, kw_args);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

STATIC mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->alarm_id != ALARM_ID_INVALID) {
        alarm_pool_cancel_alarm(self->pool, self->alarm_id);
        self->alarm_id = ALARM_ID_INVALID;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

STATIC const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_timer_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT), MP_ROM_INT(TIMER_MODE_ONE_SHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_MODE_PERIODIC) },
};
STATIC MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    print, machine_timer_print,
    locals_dict, &machine_timer_locals_dict
    );
