/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "pin.h"

const mp_obj_type_t pin_cpu_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_cpu,
    .locals_dict = (mp_obj_t)&pin_cpu_pins_locals_dict,
};

const mp_obj_type_t pin_board_pins_obj_type = {
    { &mp_type_type },
    .name = MP_QSTR_board,
    .locals_dict = (mp_obj_t)&pin_board_pins_locals_dict,
};

const pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name) {
    mp_map_t *named_map = mp_obj_dict_get_map((mp_obj_t)named_pins);
    mp_map_elem_t *named_elem = mp_map_lookup(named_map, name, MP_MAP_LOOKUP);
    if (named_elem != NULL && named_elem->value != NULL) {
        return named_elem->value;
    }
    return NULL;
}

const pin_af_obj_t *pin_find_af(const pin_obj_t *pin, uint8_t fn, uint8_t unit) {
    const pin_af_obj_t *af = pin->af;
    for (mp_uint_t i = 0; i < pin->num_af; i++, af++) {
        if (af->fn == fn && af->unit == unit) {
            return af;
        }
    }
    return NULL;
}

const pin_af_obj_t *pin_find_af_by_index(const pin_obj_t *pin, mp_uint_t af_idx) {
    const pin_af_obj_t *af = pin->af;
    for (mp_uint_t i = 0; i < pin->num_af; i++, af++) {
        if (af->idx == af_idx) {
            return af;
        }
    }
    return NULL;
}

/* unused
const pin_af_obj_t *pin_find_af_by_name(const pin_obj_t *pin, const char *name) {
    const pin_af_obj_t *af = pin->af;
    for (mp_uint_t i = 0; i < pin->num_af; i++, af++) {
        if (strcmp(name, qstr_str(af->name)) == 0) {
            return af;
        }
    }
    return NULL;
}
*/
