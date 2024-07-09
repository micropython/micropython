// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 microDev
//
// SPDX-License-Identifier: MIT

#include "bindings/espulp/ULPAlarm.h"

#include "py/runtime.h"

//| class ULPAlarm:
//|     """Trigger an alarm when the ULP requests wake-up."""
//|
//|     def __init__(self, ulp: ULP) -> None:
//|         """Create an alarm that will be triggered when the ULP requests wake-up.
//|
//|         The alarm is not active until it is passed to an `alarm`-enabling function, such as
//|         `alarm.light_sleep_until_alarms()` or `alarm.exit_and_deep_sleep_until_alarms()`.
//|
//|         :param ULP ulp: The ulp instance"""
//|         ...
//|
static mp_obj_t espulp_ulpalarm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_ulp };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ulp, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    espulp_ulpalarm_obj_t *self = mp_obj_malloc(espulp_ulpalarm_obj_t, &espulp_ulpalarm_type);

    espulp_ulp_obj_t *ulp = mp_arg_validate_type(args[ARG_ulp].u_obj, &espulp_ulp_type, MP_QSTR_ulp);

    common_hal_espulp_ulpalarm_construct(self, ulp);

    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_OBJ_TYPE(
    espulp_ulpalarm_type,
    MP_QSTR_ULPAlarm,
    MP_TYPE_FLAG_NONE,
    make_new, espulp_ulpalarm_make_new
    );
