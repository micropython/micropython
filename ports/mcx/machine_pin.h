/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 * Copyright (c) 2024 NXP
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

#ifndef MP_PORT_MCX_MACHINE_PIN_H
#define MP_PORT_MCX_MACHINE_PIN_H

#include "shared/runtime/mpirq.h"
#include "py/obj.h"

#include "machine_pin_defs.h"

typedef struct {
    uint8_t af_id; /* AF0 - AF15 */

    machine_pin_af_type_t af_type;
    uint8_t af_instance_id;
    machine_pin_af_attr_t af_attribute; /* Identify what signal this is. */
} machine_pin_af_t;

typedef struct {
    uint8_t adc_id;
    char adc_side;
    uint8_t adc_channel;
} machine_pin_adc_t;

typedef struct {
    mp_obj_base_t base;
    qstr name;

    uint8_t port;
    uint8_t pin;

    uint8_t af_count;
    const machine_pin_af_t *af;

    const machine_pin_adc_t *adc;
} machine_pin_obj_t;

#include "genhdr/pins.h"

extern const mp_obj_type_t machine_pin_board_pins_obj_type;
extern const mp_obj_type_t machine_pin_cpu_pins_obj_type;

extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;
extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;

#endif
