/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "rtc.h"
#include "hal_rtc.h"

#if MICROPY_PY_MACHINE_RTC

typedef struct _machine_rtc_obj_t {
    mp_obj_base_t    base;
    hal_rtc_conf_t * p_config;
    mp_obj_t callback;
    mp_int_t period;
    mp_int_t mode;
} machine_rtc_obj_t;

static hal_rtc_conf_t rtc_config0 = {.id = 0};
static hal_rtc_conf_t rtc_config1 = {.id = 1};
#if NRF52
static hal_rtc_conf_t rtc_config2 = {.id = 2};
#endif

STATIC machine_rtc_obj_t machine_rtc_obj[] = {
    {{&machine_rtc_type}, &rtc_config0},
    {{&machine_rtc_type}, &rtc_config1},
#if NRF52
    {{&machine_rtc_type}, &rtc_config2},
#endif
};

STATIC void hal_interrupt_handle(uint8_t id) {
    machine_rtc_obj_t * self = &machine_rtc_obj[id];;

    mp_call_function_1(self->callback, self);

    if (self != NULL) {
        hal_rtc_stop(id);
        if (self->mode == 1) {
            hal_rtc_start(id);
        }
    }
}

void rtc_init0(void) {
    hal_rtc_callback_set(hal_interrupt_handle);
}

STATIC int rtc_find(mp_obj_t id) {
    // given an integer id
    int rtc_id = mp_obj_get_int(id);
    if (rtc_id >= 0 && rtc_id <= MP_ARRAY_SIZE(machine_rtc_obj)
        && machine_rtc_obj[rtc_id].p_config != NULL) {
        return rtc_id;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "RTC(%d) does not exist", rtc_id));
}

STATIC void rtc_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_rtc_obj_t *self = o;
    mp_printf(print, "RTC(%u)", self->p_config->id);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,       MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
        { MP_QSTR_period,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000} },
        { MP_QSTR_mode,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_callback, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get static peripheral object
    int rtc_id = rtc_find(args[0].u_obj);

    // unconst machine object in order to set a callback.
    machine_rtc_obj_t * self = (machine_rtc_obj_t *)&machine_rtc_obj[rtc_id];

    self->p_config->period = args[1].u_int;

    self->mode = args[2].u_int;

    if (args[3].u_obj != mp_const_none) {
        self->callback = args[3].u_obj;
    }

#ifdef NRF51
    self->p_config->irq_priority = 3;
#else
    self->p_config->irq_priority = 6;
#endif

    hal_rtc_init(self->p_config);

    return MP_OBJ_FROM_PTR(self);
}

/// \method start(period)
/// Start the RTC timer. Timeout occurs after number of periods
/// in the configured frequency has been reached.
///
STATIC mp_obj_t machine_rtc_start(mp_obj_t self_in) {
    machine_rtc_obj_t * self = MP_OBJ_TO_PTR(self_in);

    hal_rtc_start(self->p_config->id);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_start_obj, machine_rtc_start);

/// \method stop()
/// Stop the RTC timer.
///
STATIC mp_obj_t machine_rtc_stop(mp_obj_t self_in) {
    machine_rtc_obj_t * self = MP_OBJ_TO_PTR(self_in);

    hal_rtc_stop(self->p_config->id);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_stop_obj, machine_rtc_stop);


STATIC const mp_rom_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&machine_rtc_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),  MP_ROM_PTR(&machine_rtc_stop_obj) },

    // constants
    { MP_ROM_QSTR(MP_QSTR_ONESHOT),  MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(1) },
};

STATIC MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

const mp_obj_type_t machine_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .print = rtc_print,
    .make_new = machine_rtc_make_new,
    .locals_dict = (mp_obj_dict_t*)&machine_rtc_locals_dict
};

#endif // MICROPY_PY_MACHINE_RTC
