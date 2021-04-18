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

#include "py/runtime.h"
#include "py/mphal.h"
#include "pin.h"

STATIC void pin_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
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
STATIC mp_obj_t pin_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);      // Todo: machine_pin - implement additional arguments!

    const machine_pin_obj_t *pin = pin_find(args[0]);

    // Todo: machine_pin- initialize pin if more arguments are present!

    return (mp_obj_t)pin;
}


const mp_obj_type_t machine_pin_type = {
    {&mp_type_type},
    .name = MP_QSTR_PIN,
    .print = pin_obj_print,
    .make_new = pin_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&pin_locals_dict,
};
