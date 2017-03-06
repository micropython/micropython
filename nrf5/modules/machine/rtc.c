/*
 * This file is part of the Micro Python project, http://micropython.org/
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
    mp_obj_base_t base;
    RTC_HandleTypeDef *rtc;
    mp_obj_t callback;
    mp_int_t period;
    mp_int_t mode;
} machine_rtc_obj_t;

RTC_HandleTypeDef RTCHandle0 = {.config.p_instance = NULL, .id = 0};
RTC_HandleTypeDef RTCHandle1 = {.config.p_instance = NULL, .id = 1};

STATIC machine_rtc_obj_t machine_rtc_obj[] = {
    {{&machine_rtc_type}, &RTCHandle0},
    {{&machine_rtc_type}, &RTCHandle1},
};

STATIC void hal_interrupt_handle(NRF_RTC_Type * p_instance) {
	const machine_rtc_obj_t * self = NULL;
    if (p_instance == RTC0) {
        self = &machine_rtc_obj[0];
        mp_call_function_0(self->callback);
    } else if (p_instance == RTC1) {
        self = &machine_rtc_obj[1];
        mp_call_function_0(self->callback);
    }
#if NRF52
    else if (p_instance == RTC2) {
        self = &machine_rtc_obj[2];
        mp_call_function_0(self->callback);
    }
#endif

    if (self != NULL) {
        hal_rtc_stop(&self->rtc->config);
        if (self->mode == 1) {
            hal_rtc_start(&self->rtc->config, self->period);
        }
    }
}

void rtc_init0(void) {

    hal_rtc_callback_set(hal_interrupt_handle);

    // reset the RTC handles
    memset(&RTCHandle0, 0, sizeof(RTC_HandleTypeDef));
    RTCHandle0.config.p_instance = RTC0;
    RTCHandle0.config.irq_num    = RTC0_IRQ_NUM;
#if (BLUETOOTH_SD == 100)
    RTCHandle0.config.irq_priority = 3;
#else
    RTCHandle0.config.irq_priority = 6;
#endif
    memset(&RTCHandle1, 0, sizeof(RTC_HandleTypeDef));
    RTCHandle1.config.p_instance = RTC1;
    RTCHandle1.config.irq_num    = RTC1_IRQ_NUM;
#if (BLUETOOTH_SD == 100)
    RTCHandle1.config.irq_priority = 3;
#else
    RTCHandle1.config.irq_priority = 6;
#endif
}

STATIC int rtc_find(mp_obj_t id) {
    // given an integer id
    int rtc_id = mp_obj_get_int(id);
    if (rtc_id >= 0 && rtc_id <= MP_ARRAY_SIZE(machine_rtc_obj)
        && machine_rtc_obj[rtc_id].rtc != NULL) {
        return rtc_id;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "RTC(%d) does not exist", rtc_id));
}

STATIC void rtc_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    machine_rtc_obj_t *self = o;
    mp_printf(print, "RTC(%u)", self->rtc->id);
}

/******************************************************************************/
/* MicroPython bindings for machine API                                       */

/*
from machine import RTC
def cb():
    print("Callback")
r = RTC(0, 8, cb, mode=RTC.PERIODIC)
r.start(16)
*/

STATIC mp_obj_t machine_rtc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id,        MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_frequency, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_callback,  MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_mode,      MP_ARG_KW_ONLY  | MP_ARG_INT, {.u_int = 0} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[0].u_obj == MP_OBJ_NEW_SMALL_INT(-1)) {
        // index -1 does not exist
        return mp_const_none;
        // TODO: raise exception
    }

    // get static peripheral object
    int rtc_id = rtc_find(args[0].u_obj);

    // unconst machine object in order to set a callback.
    machine_rtc_obj_t * self = (machine_rtc_obj_t *)&machine_rtc_obj[rtc_id];

    mp_obj_t freq_obj = args[1].u_obj;

    if (freq_obj != mp_const_none && MP_OBJ_IS_INT(freq_obj)) {
        self->rtc->config.frequency = mp_obj_get_int(freq_obj);
    } else {
        // raise exception
    }

    if (args[2].u_obj != mp_const_none) {
        self->callback = args[2].u_obj;
    }

    self->mode = args[3].u_int;

    hal_rtc_init(&self->rtc->config);

    return MP_OBJ_FROM_PTR(self);
}

/// \method start(period)
/// Start the RTC timer. Timeout occurs after number of periods
/// in the configured frequency has been reached.
///
STATIC mp_obj_t machine_rtc_start(mp_obj_t self_in, mp_obj_t period_in) {
    machine_rtc_obj_t * self = MP_OBJ_TO_PTR(self_in);
    mp_int_t period = mp_obj_get_int(period_in);

    self->period = mp_obj_get_int(period_in);

    hal_rtc_start(&self->rtc->config, period);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(machine_rtc_start_obj, machine_rtc_start);

/// \method stop()
/// Stop the RTC timer.
///
STATIC mp_obj_t machine_rtc_stop(mp_obj_t self_in) {
    machine_rtc_obj_t * self = MP_OBJ_TO_PTR(self_in);

    hal_rtc_stop(&self->rtc->config);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_rtc_stop_obj, machine_rtc_stop);


STATIC const mp_map_elem_t machine_rtc_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_start), (mp_obj_t)(&machine_rtc_start_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stop),  (mp_obj_t)(&machine_rtc_stop_obj) },

    // constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_ONESHOT),  MP_OBJ_NEW_SMALL_INT(0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PERIODIC), MP_OBJ_NEW_SMALL_INT(1) },
};

STATIC MP_DEFINE_CONST_DICT(machine_rtc_locals_dict, machine_rtc_locals_dict_table);

const mp_obj_type_t machine_rtc_type = {
    { &mp_type_type },
    .name = MP_QSTR_RTC,
    .print = rtc_print,
    .make_new = machine_rtc_make_new,
    .locals_dict = (mp_obj_t)&machine_rtc_locals_dict
};

#endif // MICROPY_PY_MACHINE_RTC
