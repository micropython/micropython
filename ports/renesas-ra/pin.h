/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021, 2022 Renesas Electronics Corporation
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

#ifndef MICROPY_INCLUDED_RA_PIN_H
#define MICROPY_INCLUDED_RA_PIN_H

#include "shared/runtime/mpirq.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    qstr name;
    uint8_t pin;
    uint8_t bit;
    uint8_t channel;
} machine_pin_adc_obj_t;

typedef struct {
    mp_obj_base_t base;
    qstr name;
    uint8_t pin;
    const machine_pin_adc_obj_t *ad;
} machine_pin_obj_t;

// Include all of the individual pin objects
#include "genhdr/pins.h"

extern const mp_obj_type_t machine_pin_board_pins_obj_type;
extern const mp_obj_type_t machine_pin_cpu_pins_obj_type;

extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;
extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;

void machine_pin_init(void);
uint32_t pin_get_mode(const machine_pin_obj_t *pin);
uint32_t pin_get_pull(const machine_pin_obj_t *pin);
uint32_t pin_get_drive(const machine_pin_obj_t *pin);
uint32_t pin_get_af(const machine_pin_obj_t *pin);
const machine_pin_obj_t *machine_pin_find(mp_obj_t user_obj);
const machine_pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name);

#endif // MICROPY_INCLUDED_RA_PIN_H
