/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Emil Renner Berthing
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

#include "py/runtime.h"
#include "extmod/virtpin.h"

#include "pin.h"

const pin_obj_t pin_base[80] = {
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
    {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }}, {{ &pin_type }},
};

static int pin_parse(const char *str) {
    int ret;

    if (str[0] < 'A' || str[0] > 'E') {
        return -1;
    }

    ret = (str[0] - 'A') << 4;

    if (str[1] == '1' && str[2] >= '0' && str[2] <= '5') {
        if (str[3] != '\0') {
            return -1;
        }
        ret |= str[2] - '0' + 10;
    } else if (str[1] >= '0' && str[1] <= '9') {
        if (str[2] != '\0') {
            return -1;
        }
        ret |= str[1] - '0';
    } else {
        return -1;
    }

    return ret;
}

static const pin_obj_t *pin_find(mp_obj_t user_obj) {
    // If pin is SMALL_INT
    if (mp_obj_is_small_int(user_obj)) {
        uint value = MP_OBJ_SMALL_INT_VALUE(user_obj);

        if (value >= MP_ARRAY_SIZE(pin_base)) {
            mp_raise_msg_varg(&mp_type_ValueError,
                MP_ERROR_TEXT("Pin(%u) doesn't exist"), value);
        }
        return &pin_base[value];
    }

    // If a pin was provided, then use it
    if (mp_obj_is_type(user_obj, &pin_type)) {
        return MP_OBJ_TO_PTR(user_obj);
    }

    const char *str = mp_obj_str_get_str(user_obj);
    int pin = pin_parse(str);

    if (pin > 0) {
        return &pin_base[pin];
    }

    mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin('%s') doesn't exist"), str);
}

STATIC mp_obj_t pin_obj_init_helper(const pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_value, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get pin
    gpio_pin_t pin = pin_from_obj(self);

    // get mode
    uint mode = args[0].u_int;
    if (mode > 0xfU || mode == 0xcU) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %u"), mode);
    }

    // enable the peripheral clock for the port of this pin
    gpio_pin_clock_enable(pin);

    // if given, set the pin value before initialising to prevent glitches
    if (args[1].u_obj != MP_OBJ_NULL) {
        if (mp_obj_is_true(args[1].u_obj)) {
            gpio_pin_set(pin);
        } else {
            gpio_pin_clear(pin);
        }
    }

    // configure the GPIO as requested
    gpio_pin_config(pin, mode);

    return mp_const_none;
}

/// \classmethod \constructor(id, ...)
/// Create a new Pin object associated with the id.  If additional arguments are given,
/// they are used to initialise the pin.  See `init`.
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Run an argument through the mapper and return the result.
    const pin_obj_t *pin = pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return MP_OBJ_FROM_PTR(pin);
}

/// \method __str__()
/// Return a string describing the pin object.
STATIC void pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    static const qstr pin_mode_name[16] = {
        MP_QSTR_IN_ANALOG,
        MP_QSTR_PP_10MHZ,
        MP_QSTR_PP_2MHZ,
        MP_QSTR_PP_50MHZ,
        MP_QSTR_IN_FLOAT,
        MP_QSTR_OD_10MHZ,
        MP_QSTR_OD_2MHZ,
        MP_QSTR_OD_50MHZ,
        MP_QSTR_IN_PULL,
        MP_QSTR_AF_PP_10MHZ,
        MP_QSTR_AF_PP_2MHZ,
        MP_QSTR_AF_PP_50MHZ,
        0, // reserved
        MP_QSTR_AF_OD_10MHZ,
        MP_QSTR_AF_OD_2MHZ,
        MP_QSTR_AF_OD_50MHZ,
    };
    const pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_pin_t pin = pin_from_obj(self);

    mp_printf(print, "Pin('%c%u', mode=Pin.%q, value=%u)",
        'A' + gpio_pin_port_nr(pin), gpio_pin_nr(pin),
        pin_mode_name[gpio_pin_mode(pin)],
        !!gpio_pin_get(pin));
}

// fast method for getting/setting pin value
STATIC mp_obj_t pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    const pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_pin_t pin = pin_from_obj(self);

    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(!!gpio_pin_high(pin));
    } else {
        // set pin
        if (mp_obj_is_true(args[0])) {
            gpio_pin_set(pin);
        } else {
            gpio_pin_clear(pin);
        }
        return mp_const_none;
    }
}

STATIC mp_obj_t pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pin_obj_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pin_init_obj, 1, pin_obj_init);

/// \method value([value])
/// Get or set the digital logic level of the pin:
///
///   - With no argument, return 0 or 1 depending on the logic level of the pin.
///   - With `value` given, set the logic level of the pin.  `value` can be
///   anything that converts to a boolean.  If it converts to `True`, the pin
///   is set high, otherwise it is set low.
STATIC mp_obj_t pin_value(size_t n_args, const mp_obj_t *args) {
    return pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_value_obj, 1, 2, pin_value);

STATIC mp_obj_t pin_off(mp_obj_t self_in) {
    const pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_pin_clear(pin_from_obj(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_off_obj, pin_off);

STATIC mp_obj_t pin_on(mp_obj_t self_in) {
    const pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_pin_set(pin_from_obj(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_on_obj, pin_on);

STATIC const mp_rom_map_elem_t pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&pin_on_obj) },

    // Legacy names as used by pyb.Pin
    { MP_ROM_QSTR(MP_QSTR_low),     MP_ROM_PTR(&pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),    MP_ROM_PTR(&pin_on_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN_ANALOG),   MP_ROM_INT(GPIO_MODE_IN_ANALOG) },
    { MP_ROM_QSTR(MP_QSTR_PP_10MHZ),    MP_ROM_INT(GPIO_MODE_PP_10MHZ) },
    { MP_ROM_QSTR(MP_QSTR_PP_2MHZ),     MP_ROM_INT(GPIO_MODE_PP_2MHZ) },
    { MP_ROM_QSTR(MP_QSTR_PP_50MHZ),    MP_ROM_INT(GPIO_MODE_PP_50MHZ) },
    { MP_ROM_QSTR(MP_QSTR_IN_FLOAT),    MP_ROM_INT(GPIO_MODE_IN_FLOAT) },
    { MP_ROM_QSTR(MP_QSTR_OD_10MHZ),    MP_ROM_INT(GPIO_MODE_OD_10MHZ) },
    { MP_ROM_QSTR(MP_QSTR_OD_2MHZ),     MP_ROM_INT(GPIO_MODE_OD_2MHZ) },
    { MP_ROM_QSTR(MP_QSTR_OD_50MHZ),    MP_ROM_INT(GPIO_MODE_OD_50MHZ) },
    { MP_ROM_QSTR(MP_QSTR_IN_PULL),     MP_ROM_INT(GPIO_MODE_IN_PULL) },
    { MP_ROM_QSTR(MP_QSTR_AF_PP_10MHZ), MP_ROM_INT(GPIO_MODE_AF_PP_10MHZ) },
    { MP_ROM_QSTR(MP_QSTR_AF_PP_2MHZ),  MP_ROM_INT(GPIO_MODE_AF_PP_2MHZ) },
    { MP_ROM_QSTR(MP_QSTR_AF_PP_50MHZ), MP_ROM_INT(GPIO_MODE_AF_PP_50MHZ) },
    { MP_ROM_QSTR(MP_QSTR_AF_OD_10MHZ), MP_ROM_INT(GPIO_MODE_AF_OD_10MHZ) },
    { MP_ROM_QSTR(MP_QSTR_AF_OD_2MHZ),  MP_ROM_INT(GPIO_MODE_AF_OD_2MHZ) },
    { MP_ROM_QSTR(MP_QSTR_AF_OD_50MHZ), MP_ROM_INT(GPIO_MODE_AF_OD_50MHZ) },
};

STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    const pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_pin_t pin = pin_from_obj(self);

    switch (request) {
        case MP_PIN_READ:
            return !!gpio_pin_high(pin);
        case MP_PIN_WRITE:
            if (arg) {
                gpio_pin_set(pin);
            } else {
                gpio_pin_clear(pin);
            }
            return 0;
    }
    return -1;
}

STATIC const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};

const mp_obj_type_t pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pin_print,
    .make_new = mp_pin_make_new,
    .call = pin_call,
    .protocol = &pin_pin_p,
    .locals_dict = (mp_obj_dict_t *)&pin_locals_dict,
};

/* vim: set ts=4 sw=4 et: */
