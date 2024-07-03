/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Paul Sokolovsky
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

#include <string.h>
#include "py/runtime.h"

#if MICROPY_PY_MACHINE_SIGNAL

#include "extmod/modmachine.h"
#include "extmod/virtpin.h"

// Signal class

typedef struct _machine_signal_t {
    mp_obj_base_t base;
    mp_obj_t pin;
    bool invert;
} machine_signal_t;

static mp_obj_t signal_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_t pin;
    bool invert = false;

    #if defined(MICROPY_PY_MACHINE_PIN_MAKE_NEW)
    mp_pin_p_t *pin_p = NULL;

    if (n_args > 0 && mp_obj_is_obj(args[0])) {
        mp_obj_base_t *pin_base = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[0]);
        pin_p = (mp_pin_p_t *)MP_OBJ_TYPE_GET_SLOT_OR_NULL(pin_base->type, protocol);
    }

    if (pin_p == NULL) {
        // If first argument isn't a Pin-like object, we filter out "invert"
        // from keyword arguments and pass them all to the exported Pin
        // constructor to create one.
        mp_obj_t *pin_args = mp_local_alloc((n_args + n_kw * 2) * sizeof(mp_obj_t));
        memcpy(pin_args, args, n_args * sizeof(mp_obj_t));
        const mp_obj_t *src = args + n_args;
        mp_obj_t *dst = pin_args + n_args;
        mp_obj_t *sig_value = NULL;
        for (size_t cnt = n_kw; cnt; cnt--) {
            if (*src == MP_OBJ_NEW_QSTR(MP_QSTR_invert)) {
                invert = mp_obj_is_true(src[1]);
                n_kw--;
            } else {
                *dst++ = *src;
                *dst++ = src[1];
            }
            if (*src == MP_OBJ_NEW_QSTR(MP_QSTR_value)) {
                // Value is pertained to Signal, so we should invert
                // it for Pin if needed, and we should do it only when
                // inversion status is guaranteedly known.
                sig_value = dst - 1;
            }
            src += 2;
        }

        if (invert && sig_value != NULL) {
            *sig_value = mp_obj_is_true(*sig_value) ? MP_OBJ_NEW_SMALL_INT(0) : MP_OBJ_NEW_SMALL_INT(1);
        }

        // Here we pass NULL as a type, hoping that mp_pin_make_new()
        // will just ignore it as set a concrete type. If not, we'd need
        // to expose port's "default" pin type too.
        pin = MICROPY_PY_MACHINE_PIN_MAKE_NEW(NULL, n_args, n_kw, pin_args);

        mp_local_free(pin_args);
    } else
    #endif
    // Otherwise there should be 1 or 2 args
    {
        if (n_args == 1) {
            pin = args[0];
            if (n_kw == 0) {
            } else if (n_kw == 1 && args[1] == MP_OBJ_NEW_QSTR(MP_QSTR_invert)) {
                invert = mp_obj_is_true(args[2]);
            } else {
                goto error;
            }
        } else {
        error:
            mp_raise_TypeError(NULL);
        }
    }

    machine_signal_t *o = mp_obj_malloc(machine_signal_t, type);
    o->pin = pin;
    o->invert = invert;
    return MP_OBJ_FROM_PTR(o);
}

static mp_uint_t signal_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_signal_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
        case MP_PIN_READ: {
            return mp_virtual_pin_read(self->pin) ^ self->invert;
        }
        case MP_PIN_WRITE: {
            mp_virtual_pin_write(self->pin, arg ^ self->invert);
            return 0;
        }
    }
    return -1;
}

// fast method for getting/setting signal value
static mp_obj_t signal_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(mp_virtual_pin_read(self_in));
    } else {
        // set pin
        mp_virtual_pin_write(self_in, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

static mp_obj_t signal_value(size_t n_args, const mp_obj_t *args) {
    return signal_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(signal_value_obj, 1, 2, signal_value);

static mp_obj_t signal_on(mp_obj_t self_in) {
    mp_virtual_pin_write(self_in, 1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(signal_on_obj, signal_on);

static mp_obj_t signal_off(mp_obj_t self_in) {
    mp_virtual_pin_write(self_in, 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(signal_off_obj, signal_off);

static const mp_rom_map_elem_t signal_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&signal_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&signal_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&signal_off_obj) },
};

static MP_DEFINE_CONST_DICT(signal_locals_dict, signal_locals_dict_table);

static const mp_pin_p_t signal_pin_p = {
    .ioctl = signal_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_signal_type,
    MP_QSTR_Signal,
    MP_TYPE_FLAG_NONE,
    make_new, signal_make_new,
    call, signal_call,
    protocol, &signal_pin_p,
    locals_dict, &signal_locals_dict
    );

#endif // MICROPY_PY_MACHINE_SIGNAL
