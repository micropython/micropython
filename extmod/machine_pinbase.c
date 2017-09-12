/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

STATIC mp_obj_t pinbase_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)args;
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
    .ioctl = pinbase_ioctl,
};

const mp_obj_type_t machine_pinbase_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinBase,
    .make_new = pinbase_make_new,
    .protocol = &pinbase_pin_p,
};

#endif // MICROPY_PY_MACHINE
