/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include MICROPY_HAL_H
#include "pin.h"

STATIC void pin_named_pins_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_named_pins_obj_t *self = self_in;
    print(env, "<Pin.%s>", qstr_str(self->name));
}

STATIC void pin_named_pins_obj_load_attr(mp_obj_t self_in, qstr attr_qstr, mp_obj_t *dest) {
    pin_named_pins_obj_t *self = self_in;
    const char *attr = qstr_str(attr_qstr);
    const pin_obj_t *pin = pin_find_named_pin(self->named_pins, attr);
    if (pin) {
        dest[0] = (mp_obj_t)pin;
        dest[1] = MP_OBJ_NULL;
    }
}

static const mp_obj_type_t pin_named_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_PinNamed,
    .print = pin_named_pins_obj_print,
    .load_attr = pin_named_pins_obj_load_attr,
};

const pin_named_pins_obj_t pin_board_pins_obj = {
    { &pin_named_pins_obj_type },
    .name = MP_QSTR_board,
    .named_pins = pin_board_pins,
};

const pin_named_pins_obj_t pin_cpu_pins_obj = {
    { &pin_named_pins_obj_type },
    .name = MP_QSTR_cpu,
    .named_pins = pin_cpu_pins,
};

const pin_obj_t *pin_find_named_pin(const pin_named_pin_t *named_pins, const char *name) {
    const pin_named_pin_t *named_pin = named_pins;
    while (named_pin->name) {
        if (!strcmp(name, named_pin->name)) {
            return named_pin->pin;
        }
        named_pin++;
    }
    return NULL;
}

const pin_af_obj_t *pin_find_af(const pin_obj_t *pin, uint8_t fn, uint8_t unit, uint8_t type) {
    const pin_af_obj_t *af = pin->af;
    for (int i = 0; i < pin->num_af; i++, af++) {
        if (af->fn == fn && af->unit == unit && af->type == type) {
            return af;
        }
    }
    return NULL;
}
