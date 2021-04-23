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

#ifndef MICROPY_INCLUDED_MIMXRT_PIN_H
#define MICROPY_INCLUDED_MIMXRT_PIN_H

#include <stdint.h>
#include "fsl_gpio.h"
#include "py/obj.h"

// ------------------------------------------------------------------------------------------------------------------ //

enum {
    PIN_MODE_IN = 0,
    PIN_MODE_OUT,
    PIN_MODE_ALT,
};

enum {
    PIN_AF_MODE_ALT1 = 1,
    PIN_AF_MODE_ALT2,
    PIN_AF_MODE_ALT3,
    PIN_AF_MODE_ALT4,
    PIN_AF_MODE_ALT5,
    PIN_AF_MODE_ALT6,
    PIN_AF_MODE_ALT7,
    PIN_AF_MODE_ALT8,
};

// ------------------------------------------------------------------------------------------------------------------ //

typedef struct {
    mp_obj_base_t base;
    qstr name;  // port name
    uint32_t af_mode;  // alternate function
    void *instance;  // pointer to peripheral instance for alternate function
    uint32_t pad_config;  // pad configuration for alternate function
} machine_pin_af_obj_t;

typedef struct {
    mp_obj_base_t base;
    qstr name;  // pad name
    GPIO_Type *gpio;  // gpio instance for pin
    uint32_t pin;  // pin number
    uint32_t muxRegister;
    uint32_t configRegister;
    uint32_t mode;  // current pin mode
    uint32_t af_mode;  // current alternate function mode
    size_t af_list_len;  // length of available alternate functions list
    const machine_pin_af_obj_t *af_list;  // pointer tolist with alternate functions
} machine_pin_obj_t;

// ------------------------------------------------------------------------------------------------------------------ //

extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_pin_af_type;

// ------------------------------------------------------------------------------------------------------------------ //

// Include board specific pins
#include "genhdr/pins.h"  // pins.h must included at this location

// ------------------------------------------------------------------------------------------------------------------ //

void pin_init(void);
uint32_t pin_get_mode(const machine_pin_obj_t *pin);
uint32_t pin_get_pull(const machine_pin_obj_t *pin);
uint32_t pin_get_af(const machine_pin_obj_t *pin);
const machine_pin_obj_t *pin_find(mp_obj_t user_obj);
const machine_pin_obj_t *pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name);
const machine_pin_af_obj_t *pin_find_af(const machine_pin_obj_t *pin, uint8_t fn, uint8_t unit);
const machine_pin_af_obj_t *pin_find_af_by_index(const machine_pin_obj_t *pin, mp_uint_t af_idx);
const machine_pin_af_obj_t *pin_find_af_by_name(const machine_pin_obj_t *pin, const char *name);

#endif // MICROPY_INCLUDED_MIMXRT_PIN_H
