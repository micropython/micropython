/*
 * This is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2021 Damien P. George
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
 *
 * Uses pins.h & pins.c to create board (MCU package) specific 'machine_led_obj' array.
 */

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/virtpin.h"
#include "modmachine.h"
#include "pins.h" // boards/<BOARD>/

// ASF4 (MCU package specific pin defs in 'boards')
#include "hal_gpio.h"
#include "hpl_gpio.h"
#include "hal_atomic.h"

STATIC void machine_led_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_led_obj_t *self = self_in;
    mp_printf(print, "LED(%u)", self->id);
}

// constructor(id, ...)
mp_obj_t mp_led_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get the wanted LED object
    int wanted_led = pin_find(args[0], (const machine_pin_obj_t *)machine_led_obj, MP_ARRAY_SIZE(machine_led_obj));
    const machine_led_obj_t *self = NULL;
    if (0 <= wanted_led && wanted_led < MP_ARRAY_SIZE(machine_led_obj)) {
        self = (machine_led_obj_t *)&machine_led_obj[wanted_led];
    }

    // the array could be padded with 'nulls' (see other Ports).
    // Will also error if the asked for LED (index) is greater than the array row size.
    if (self == NULL || self->base.type == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid LED"));
    }
    mp_hal_pin_output(self->id);
    mp_hal_pin_low(self->id);

    return MP_OBJ_FROM_PTR(self);
}

// pin.low()
STATIC mp_obj_t machine_led_low(mp_obj_t self_in) {
    machine_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_low(self->id);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_led_low_obj, machine_led_low);

// pin.high()
STATIC mp_obj_t machine_led_high(mp_obj_t self_in) {
    machine_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_high(self->id);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_led_high_obj, machine_led_high);

// pin.toggle()
STATIC mp_obj_t machine_led_toggle(mp_obj_t self_in) {
    machine_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    gpio_toggle_pin_level(self->id);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_led_toggle_obj, machine_led_toggle);

STATIC const mp_rom_map_elem_t machine_led_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&machine_led_low_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&machine_led_high_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&machine_led_toggle_obj) },
};
STATIC MP_DEFINE_CONST_DICT(machine_led_locals_dict, machine_led_locals_dict_table);

const mp_obj_type_t machine_led_type = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = machine_led_print,
    .make_new = mp_led_make_new,
    .locals_dict = (mp_obj_t)&machine_led_locals_dict,
};
