// Copyright (c) 2016 Paul Sokolovsky
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
//
// SPDX-License-Identifier: MIT

#include "py/mpconfig.h"
#if MICROPY_PY_MACHINE

#include "py/obj.h"
#include "py/runtime.h"
#include "extmod/virtpin.h"
#include "extmod/machine_pinbase.h"

// PinBase class

// As this is abstract class, its instance is null.
// But there should be an instance, as the rest of instance code
// expects that there will be concrete object for inheritance.
typedef struct _mp_pinbase_t {
    mp_obj_base_t base;
} mp_pinbase_t;

STATIC const mp_pinbase_t pinbase_singleton = {
    .base = { &machine_pinbase_type },
};

STATIC mp_obj_t pinbase_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    (void)type;
    (void)n_args;
    (void)args;
    (void)kw_args;
    return MP_OBJ_FROM_PTR(&pinbase_singleton);
}

mp_uint_t pinbase_ioctl(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode);
mp_uint_t pinbase_ioctl(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    switch (request) {
        case MP_PIN_READ: {
            mp_obj_t dest[2];
            mp_load_method(obj, MP_QSTR_value, dest);
            return mp_obj_get_int(mp_call_method_n_kw(0, 0, dest));
        }
        case MP_PIN_WRITE: {
            mp_obj_t dest[3];
            mp_load_method(obj, MP_QSTR_value, dest);
            dest[2] = (arg == 0 ? mp_const_false : mp_const_true);
            mp_call_method_n_kw(1, 0, dest);
            return 0;
        }
    }
    return -1;
}

STATIC const mp_pin_p_t pinbase_pin_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_pin)
    .ioctl = pinbase_ioctl,
};

const mp_obj_type_t machine_pinbase_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinBase,
    .make_new = pinbase_make_new,
    .protocol = &pinbase_pin_p,
};

#endif // MICROPY_PY_MACHINE
