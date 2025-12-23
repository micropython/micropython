/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_PSOC_EDGE_MACHINE_PIN_H
#define MICROPY_INCLUDED_PSOC_EDGE_MACHINE_PIN_H

#include "py/obj.h"

enum {GPIO_MODE_NONE = 0, GPIO_MODE_IN, GPIO_MODE_OUT, GPIO_MODE_OPEN_DRAIN};

enum {GPIO_PULL_NONE = 0, GPIO_PULL_UP, GPIO_PULL_DOWN, GPIO_PULL_UP_DOWN};

enum {GPIO_IRQ_LEVEL_NONE=0, GPIO_IRQ_RISING, GPIO_IRQ_FALLING};

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    qstr name;
    uint8_t port;
    uint8_t pin;
} machine_pin_obj_t;

extern const mp_obj_type_t machine_pin_type;

#include "genhdr/pins.h"

extern const mp_obj_dict_t machine_pin_cpu_pins_locals_dict;
extern const mp_obj_dict_t machine_pin_board_pins_locals_dict;

const machine_pin_obj_t *machine_pin_get_pin_obj(mp_obj_t obj);

#endif // MICROPY_INCLUDED_PSOC_EDGE_MACHINE_PIN_H
