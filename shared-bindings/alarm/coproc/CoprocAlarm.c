/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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

#if CIRCUITPY_COPROC
#include "shared-bindings/util.h"
#include "shared-bindings/alarm/coproc/CoprocAlarm.h"
#include "shared-bindings/coproc/Coproc.h"

STATIC coproc_coproc_obj_t *get_coproc_obj(mp_obj_t *self_in) {
    if (!mp_obj_is_type(*self_in, &coproc_coproc_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), MP_QSTR_Coproc);
    }
    coproc_coproc_obj_t *self = MP_OBJ_TO_PTR(*self_in);
    if (common_hal_coproc_coproc_deinited(self)) {
        raise_deinited_error();
    }
    return self;
}
#endif

//| class CoprocAlarm:
//|     """Trigger an alarm when another core or co-processor requests wake-up."""
//|
//|     def __init__(self, coproc: coproc.Coproc) -> None:
//|         """Create an alarm that will be triggered when the co-processor requests wake-up.
//|
//|         The alarm is not active until it is passed to an `alarm`-enabling function, such as
//|         `alarm.light_sleep_until_alarms()` or `alarm.exit_and_deep_sleep_until_alarms()`.
//|
//|         :param coproc.Coproc coproc: The coproc program to run.
//|
//|         """
//|         ...
//|
STATIC mp_obj_t alarm_coproc_coprocalarm_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_coproc };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_coproc, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if CIRCUITPY_COPROC
    // initialize CoprocAlarm object
    alarm_coproc_coprocalarm_obj_t *self = m_new_obj(alarm_coproc_coprocalarm_obj_t);
    self->base.type = &alarm_coproc_coprocalarm_type;
    self->coproc = get_coproc_obj(&args[ARG_coproc].u_obj);
    // return CoprocAlarm object
    return MP_OBJ_FROM_PTR(self);
    #else
    mp_raise_NotImplementedError(NULL);
    return mp_const_none;
    #endif
}

const mp_obj_type_t alarm_coproc_coprocalarm_type = {
    { &mp_type_type },
    .name = MP_QSTR_CoprocAlarm,
    .make_new = alarm_coproc_coprocalarm_make_new,
};
