/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Philipp Ebensberger
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

#include "fsl_gpio.h"
#include "fsl_iomuxc.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "pin.h"
#include "mphalport.h"


// Helper Functions
STATIC mp_obj_t pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL}},
        // TODO: Implement additional arguments
        /*{ MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE}},
        { MP_QSTR_af, MP_ARG_INT, {.u_int = -1}}, // legacy
        { MP_QSTR_alt, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1}},*/
    };

    // Parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get io mode
    uint mode = args[0].u_int;
    if (!IS_GPIO_MODE(mode)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("invalid pin mode: %d"), mode);
    }

    // Handle configuration for GPIO
    if ((mode == PIN_MODE_IN) || (mode == PIN_MODE_OUT)) {
        gpio_pin_config_t pin_config;
        const machine_pin_af_obj_t *af_obj;

        pin_config.outputLogic = args[1].u_int;
        pin_config.direction = mode == PIN_MODE_IN ? kGPIO_DigitalInput : kGPIO_DigitalOutput;
        pin_config.interruptMode = kGPIO_NoIntmode;

        af_obj = pin_find_af(self, PIN_AF_MODE_ALT5);  // GPIO is always ALT5
        if (af_obj == NULL) {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("requested AF %d not available for pin %d"), PIN_AF_MODE_ALT5, mode);
        }

        // Update machine pin object
        self->mode = mode;

        // Configure pad as GPIO
        GPIO_PinInit(self->gpio, self->pin, &pin_config);
        IOMUXC_SetPinMux(self->muxRegister, af_obj->af_mode, 0, 0, self->configRegister, 1U);  // Software Input On Field: Input Path is determined by functionality
        IOMUXC_SetPinConfig(self->muxRegister, af_obj->af_mode, 0, 0, self->configRegister, af_obj->pad_config);  // TODO: use correct AF settings from AF list
    }

    return mp_const_none;
}


STATIC void machine_pin_named_pins_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "<Pin.%q>", self->name);
}

const mp_obj_type_t machine_pin_cpu_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_cpu,
    .print = machine_pin_named_pins_obj_print,
    .locals_dict = (mp_obj_t)&machine_pin_cpu_pins_locals_dict,
};

const mp_obj_type_t machine_pin_board_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_board,
    .print = machine_pin_named_pins_obj_print,
    .locals_dict = (mp_obj_t)&machine_pin_board_pins_locals_dict,
};


STATIC void machine_pin_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    (void)kind;
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(o);
    mp_printf(print, "PIN(%s)", qstr_str(self->name));
}

/**
 * pin_obj_make_new
 *
 *	pin = machine.Pin(id)
 *
 *	with id being either:
 *		int: board pin numbers
 *		str: board or cpu pin names
 */
STATIC mp_obj_t machine_pin_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    machine_pin_obj_t *pin = pin_find(args[0]);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

STATIC mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    mp_hal_pin_low(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

STATIC mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    mp_hal_pin_high(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

STATIC mp_obj_t machine_pin_value(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_value_obj, machine_pin_value);

STATIC mp_obj_t machine_pin_mode(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
    return mp_obj_new_int(self->mode);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_mode_obj, machine_pin_mode);


STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // TODO: Implement class locals dictionary
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_low),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),    MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_mode),    MP_ROM_PTR(&machine_pin_mode_obj) },
    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&machine_pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu),     MP_ROM_PTR(&machine_pin_cpu_pins_obj_type) },
    // class constants
    { MP_ROM_QSTR(MP_QSTR_IN),      MP_ROM_INT(PIN_MODE_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT),     MP_ROM_INT(PIN_MODE_OUT) },
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);


const mp_obj_type_t machine_pin_type = {
    {&mp_type_type},
    .name = MP_QSTR_Pin,
    .print = machine_pin_obj_print,
    .make_new = machine_pin_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pin_locals_dict,
};

// FIXME: Create actual pin_af type!!!
const mp_obj_type_t machine_pin_af_type = {
    {&mp_type_type},
    .name = MP_QSTR_PinAF,
    .print = machine_pin_obj_print,
    .make_new = machine_pin_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pin_locals_dict,
};
