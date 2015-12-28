/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014, 2015 Damien P. George
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
#include <stdint.h>
#include <string.h>

#include "c_types.h"
#include "user_interface.h"
#include "gpio.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "modpyb.h"

#define GPIO_MODE_INPUT (0)
#define GPIO_MODE_OUTPUT (1)
#define GPIO_MODE_OPEN_DRAIN (2) // synthesised
#define GPIO_PULL_NONE (0)
#define GPIO_PULL_UP (1)
// Removed in SDK 1.1.0
//#define GPIO_PULL_DOWN (2)

typedef struct _pyb_pin_obj_t {
    mp_obj_base_t base;
    uint16_t pin_id;
    uint16_t phys_port;
    uint32_t periph;
    uint16_t func;
} pyb_pin_obj_t;

STATIC const pyb_pin_obj_t pyb_pin_obj[] = {
    {{&pyb_pin_type}, 0, 0, PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0},
    {{&pyb_pin_type}, 1, 1, PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1},
    {{&pyb_pin_type}, 2, 2, PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2},
    {{&pyb_pin_type}, 3, 3, PERIPHS_IO_MUX_U0RXD_U, FUNC_GPIO3},
    {{&pyb_pin_type}, 4, 4, PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4},
    {{&pyb_pin_type}, 5, 5, PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5},
    {{&pyb_pin_type}, 9, 9, PERIPHS_IO_MUX_SD_DATA2_U, FUNC_GPIO9},
    {{&pyb_pin_type}, 10, 10, PERIPHS_IO_MUX_SD_DATA3_U, FUNC_GPIO10},
    {{&pyb_pin_type}, 12, 12, PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12},
    {{&pyb_pin_type}, 13, 13, PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13},
    {{&pyb_pin_type}, 14, 14, PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14},
    {{&pyb_pin_type}, 15, 15, PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15},
};

STATIC uint8_t pin_mode[16];

STATIC void pin_set(uint pin, uint mode, int value) {
    uint32_t enable = 0;
    uint32_t disable = 0;
    switch (mode) {
        case GPIO_MODE_INPUT:
            value = -1;
            disable = 1;
            break;

        case GPIO_MODE_OUTPUT:
            enable = 1;
            break;

        case GPIO_MODE_OPEN_DRAIN:
            if (value == -1) {
                return;
            } else if (value == 0) {
                enable = 1;
            } else {
                value = -1;
                disable = 1;
            }
            break;
    }

    enable <<= pin;
    disable <<= pin;
    if (value == -1) {
        gpio_output_set(0, 0, enable, disable);
    } else {
        gpio_output_set(value << pin, (1 - value) << pin, enable, disable);
    }
}

STATIC void pyb_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_pin_obj_t *self = self_in;

    // pin name
    mp_printf(print, "Pin(%u)", self->pin_id);
}

// pin.init(mode, pull=Pin.PULL_NONE, af=-1)
STATIC mp_obj_t pyb_pin_obj_init_helper(pyb_pin_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_pull, MP_ARG_INT, {.u_int = GPIO_PULL_NONE}},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
    };

    // parse args
    struct {
        mp_arg_val_t mode, pull, value;
    } args;
    mp_arg_parse_all(n_args, pos_args, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, (mp_arg_val_t*)&args);

    // get io mode
    uint mode = args.mode.u_int;

    // get pull mode
    uint pull = args.pull.u_int;

    // get initial value
    int value;
    if (args.value.u_obj == MP_OBJ_NULL) {
        value = -1;
    } else {
        value = mp_obj_is_true(args.value.u_obj);
    }

    // save the mode
    pin_mode[self->phys_port] = mode;

    // configure the GPIO as requested
    PIN_FUNC_SELECT(self->periph, self->func);
    #if 0
    // Removed in SDK 1.1.0
    if ((pull & GPIO_PULL_DOWN) == 0) {
        PIN_PULLDWN_DIS(self->periph);
    }
    #endif
    if ((pull & GPIO_PULL_UP) == 0) {
        PIN_PULLUP_DIS(self->periph);
    }
    #if 0
    if ((pull & GPIO_PULL_DOWN) != 0) {
        PIN_PULLDWN_EN(self->periph);
    }
    #endif
    if ((pull & GPIO_PULL_UP) != 0) {
        PIN_PULLUP_EN(self->periph);
    }

    pin_set(self->phys_port, mode, value);

    return mp_const_none;
}

// constructor(id, ...)
STATIC mp_obj_t pyb_pin_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Run an argument through the mapper and return the result.
    int wanted_pin = mp_obj_get_int(args[0]);
    pyb_pin_obj_t *pin = NULL;
    for (int i = 0; i < MP_ARRAY_SIZE(pyb_pin_obj); i++) {
        if (pyb_pin_obj[i].pin_id == wanted_pin) {
            pin = (pyb_pin_obj_t*)&pyb_pin_obj[i];
            break;
        }
    }
    if (pin == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid pin"));
    }

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pyb_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

// fast method for getting/setting pin value
STATIC mp_obj_t pyb_pin_call(mp_obj_t self_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    pyb_pin_obj_t *self = self_in;
    if (n_args == 0) {
        // get pin
        return MP_OBJ_NEW_SMALL_INT(GPIO_INPUT_GET(self->phys_port));
    } else {
        // set pin
        pin_set(self->phys_port, pin_mode[self->phys_port], mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

// pin.init(mode, pull)
STATIC mp_obj_t pyb_pin_obj_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return pyb_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_pin_init_obj, 1, pyb_pin_obj_init);

// pin.value([value])
STATIC mp_obj_t pyb_pin_value(mp_uint_t n_args, const mp_obj_t *args) {
    return pyb_pin_call(args[0], n_args - 1, 0, args + 1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_pin_value_obj, 1, 2, pyb_pin_value);

// pin.low()
STATIC mp_obj_t pyb_pin_low(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_set(self->phys_port, pin_mode[self->phys_port], 0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_low_obj, pyb_pin_low);

// pin.high()
STATIC mp_obj_t pyb_pin_high(mp_obj_t self_in) {
    pyb_pin_obj_t *self = self_in;
    pin_set(self->phys_port, pin_mode[self->phys_port], 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_pin_high_obj, pyb_pin_high);

STATIC const mp_map_elem_t pyb_pin_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_init),    (mp_obj_t)&pyb_pin_init_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_value),   (mp_obj_t)&pyb_pin_value_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_low),     (mp_obj_t)&pyb_pin_low_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_high),    (mp_obj_t)&pyb_pin_high_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_IN),        MP_OBJ_NEW_SMALL_INT(GPIO_MODE_INPUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OUT),       MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OUTPUT) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_OPEN_DRAIN), MP_OBJ_NEW_SMALL_INT(GPIO_MODE_OPEN_DRAIN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_NONE), MP_OBJ_NEW_SMALL_INT(GPIO_PULL_NONE) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_PULL_UP),   MP_OBJ_NEW_SMALL_INT(GPIO_PULL_UP) },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_PULL_DOWN), MP_OBJ_NEW_SMALL_INT(GPIO_PULL_DOWN) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_pin_locals_dict, pyb_pin_locals_dict_table);

const mp_obj_type_t pyb_pin_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = pyb_pin_print,
    .make_new = pyb_pin_make_new,
    .call = pyb_pin_call,
    .locals_dict = (mp_obj_t)&pyb_pin_locals_dict,
};
