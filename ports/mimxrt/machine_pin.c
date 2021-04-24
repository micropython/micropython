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
    mp_printf(print, "PIN(%s, %u)", self->name, self->pin);
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
    mp_arg_check_num(n_args, n_kw, 1, 1, false);      // Todo: machine_pin - implement additional arguments!

    const machine_pin_obj_t *pin = pin_find(args[0]);

    // Todo: machine_pin- initialize pin if more arguments are present!

    gpio_pin_config_t pin_config = {
        .outputLogic = 0U,
        .direction = kGPIO_DigitalOutput,
        .interruptMode = kGPIO_NoIntmode,
    };

    GPIO_PinInit(pin->gpio, pin->pin, &pin_config);
    IOMUXC_SetPinMux(pin->muxRegister, PIN_AF_MODE_ALT5, 0, 0, pin->configRegister, 1U);  // Software Input On Field: Input Path is determined by functionality
    IOMUXC_SetPinConfig(pin->muxRegister, PIN_AF_MODE_ALT5, 0, 0, pin->configRegister, 0x10B0U);  // TODO: use correct AF settings from AF list

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


STATIC const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // TODO: Implement class locals dictionary
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_low),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_high),    MP_ROM_PTR(&machine_pin_on_obj) },
    // class attributes
    { MP_ROM_QSTR(MP_QSTR_board),   MP_ROM_PTR(&machine_pin_board_pins_obj_type) },
    { MP_ROM_QSTR(MP_QSTR_cpu),     MP_ROM_PTR(&machine_pin_cpu_pins_obj_type) },

    // class constants
};
STATIC MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);


const mp_obj_type_t machine_pin_type = {
    {&mp_type_type},
    .name = MP_QSTR_PIN,
    .print = machine_pin_obj_print,
    .make_new = machine_pin_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pin_locals_dict,
};

// FIXME: Create actual pin_af type!!!
const mp_obj_type_t machine_pin_af_type = {
    {&mp_type_type},
    .name = MP_QSTR_PIN,
    .print = machine_pin_obj_print,
    .make_new = machine_pin_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&machine_pin_locals_dict,
};
