/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#include "py/mpconfig.h"
#include "py/obj.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "pybpin.h"
#include MICROPY_HAL_H

STATIC void pin_named_pins_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pin_named_pins_obj_t *self = self_in;
    print(env, "<Pin.%s>", qstr_str(self->name));
}

const mp_obj_type_t pin_cpu_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_cpu,
    .print = pin_named_pins_obj_print,
    .locals_dict = (mp_obj_t)&pin_cpu_pins_locals_dict,
};

const pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    mp_map_t *named_map = mp_obj_dict_get_map((mp_obj_t)named_pins);
    mp_map_elem_t *named_elem = mp_map_lookup(named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != NULL) {
        return named_elem->value;
    }
    return NULL;
}

const pin_obj_t *pin_find_pin(const mp_obj_dict_t *named_pins, uint pin_num) {
    mp_map_t *named_map = mp_obj_dict_get_map((mp_obj_t)named_pins);
    for (uint i = 0; i < named_map->used; i++) {
        if (((pin_obj_t *)named_map->table[i].value)->pin_num == pin_num) {
            return named_map->table[i].value;
        }
    }
    return NULL;
}
