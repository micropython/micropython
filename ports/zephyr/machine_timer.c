/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Daniel Campora on behalf of REMOTE TECH LTD
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

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "modmachine.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>

#define TIMER_MS_PER_TICK (1000)

typedef enum _machine_timer_mode_t {
    TIMER_MODE_ONE_SHOT = 0,
    TIMER_MODE_PERIODIC
} machine_timer_mode_t;

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;

    struct k_timer my_timer;

    machine_timer_mode_t mode;
    uint32_t period_ms;

    mp_obj_t callback;

    struct _machine_timer_obj_t *next;
} machine_timer_obj_t;

const mp_obj_type_t machine_timer_type;

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static mp_obj_t machine_timer_deinit(mp_obj_t self_in);

static void machine_timer_callback(struct k_timer *timer) {
    machine_timer_obj_t *self = (machine_timer_obj_t *)k_timer_user_data_get(timer);
    if (self->mode == TIMER_MODE_ONE_SHOT) {
        machine_timer_deinit(self);
    }
    if (self->callback != mp_const_none) {
        mp_sched_schedule(self->callback, MP_OBJ_FROM_PTR(self));
    }
}

static void machine_timer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_timer_obj_t *self = self_in;
    qstr mode_str = (self->mode == TIMER_MODE_PERIODIC) ? MP_QSTR_PERIODIC : MP_QSTR_ONE_SHOT;
    mp_printf(print, "Timer(-1, mode=%q, period=%lu)", mode_str, self->period_ms);
}

static mp_obj_t machine_timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Get timer id (only soft timer (-1) supported at the moment)
    mp_int_t id = -1;
    if (n_args > 0) {
        id = mp_obj_get_int(args[0]);
        --n_args;
        ++args;
    }
    if (id != -1) {
        mp_raise_ValueError(MP_ERROR_TEXT("only virtual timers are supported"));
    }

    // Create the new timer.
    machine_timer_obj_t *self = mp_obj_malloc_with_finaliser(machine_timer_obj_t, &machine_timer_type);

    // Add the timer to the linked-list of timers
    self->next = MP_STATE_PORT(machine_timer_obj_head);
    MP_STATE_PORT(machine_timer_obj_head) = self;

    if (n_args > 0 || n_kw > 0) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_timer_init_helper(self, n_args, args, &kw_args);
    }
    return self;
}

static mp_obj_t machine_timer_init_helper(machine_timer_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_mode,
        ARG_callback,
        ARG_period,
        ARG_freq,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = TIMER_MODE_PERIODIC} },
        { MP_QSTR_callback,     MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_period,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        #if MICROPY_PY_BUILTINS_FLOAT
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        #else
        { MP_QSTR_freq,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },
        #endif
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if MICROPY_PY_BUILTINS_FLOAT
    if (args[ARG_freq].u_obj != mp_const_none) {
        self->period_ms = (uint32_t)(TIMER_MS_PER_TICK / mp_obj_get_float(args[ARG_freq].u_obj));
    }
    #else
    if (args[ARG_freq].u_int != 0xffffffff) {
        self->period_ms = TIMER_MS_PER_TICK / ((uint64_t)args[ARG_freq].u_int);
    }
    #endif
    else {
        self->period_ms = args[ARG_period].u_int;
    }

    self->mode = args[ARG_mode].u_int;
    self->callback = args[ARG_callback].u_obj;

    k_timer_init(&self->my_timer, machine_timer_callback, NULL);
    k_timer_user_data_set(&self->my_timer, self);
    k_timer_start(&self->my_timer, K_MSEC(self->period_ms), K_MSEC(self->period_ms));

    return mp_const_none;
}

static mp_obj_t machine_timer_deinit(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    machine_timer_obj_t *prev = NULL;

    k_timer_stop(&self->my_timer);

    // remove it from the linked list
    for (machine_timer_obj_t *_timer = MP_STATE_PORT(machine_timer_obj_head); _timer != NULL; _timer = _timer->next) {
        if (_timer == self) {
            if (prev != NULL) {
                prev->next = _timer->next;
            } else {
                // move the start pointer
                MP_STATE_PORT(machine_timer_obj_head) = _timer->next;
            }
            break;
        } else {
            prev = _timer;
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_deinit_obj, machine_timer_deinit);

static mp_obj_t machine_timer_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_timer_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_timer_init_obj, 1, machine_timer_init);

static mp_obj_t machine_timer_value(mp_obj_t self_in) {
    machine_timer_obj_t *self = self_in;
    k_ticks_t ticks = k_timer_remaining_ticks(&self->my_timer);
    return MP_OBJ_NEW_SMALL_INT(k_ticks_to_ms_near32(ticks));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_timer_value_obj, machine_timer_value);

static const mp_rom_map_elem_t machine_timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__),     MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),      MP_ROM_PTR(&machine_timer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init),        MP_ROM_PTR(&machine_timer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),       MP_ROM_PTR(&machine_timer_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_ONE_SHOT),    MP_ROM_INT(TIMER_MODE_ONE_SHOT) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC),    MP_ROM_INT(TIMER_MODE_PERIODIC) },
};
static MP_DEFINE_CONST_DICT(machine_timer_locals_dict, machine_timer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_timer_type,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, machine_timer_make_new,
    print, machine_timer_print,
    locals_dict, &machine_timer_locals_dict
    );

MP_REGISTER_ROOT_POINTER(struct _machine_timer_obj_t *machine_timer_obj_head);
