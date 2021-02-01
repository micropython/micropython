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

#include "fsl_gpio.h"
#include "py/obj.h"

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

typedef struct {
    mp_obj_base_t base;
    qstr name;  // port name
    uint32_t af_mode;  // alternate function
    void *instance;  // pointer to peripheral instance for alternate function
    uint32_t pad_config;  // pad configuration for alternate function
} pin_af_obj_t;

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
    const pin_af_obj_t *af_list;  // pointer tolist with alternate functions
} pin_obj_t;

extern const mp_obj_type_t pin_type;
extern const mp_obj_type_t pin_af_type;

#define PIN_AF(_name, _af_mode, _instance, _pad_config) \
    { \
        .base = { &pin_af_type }, \
        .name = MP_QSTR_##_name, \
        .af_mode = (uint32_t)(_af_mode), \
        .instance = (void *)(_instance), \
        .pad_config = (uint32_t)(_pad_config), \
    } \

#define PIN(_name, _gpio, _pin, _af_list) \
    { \
        .base = { &pin_type }, \
        .name = MP_QSTR_##_name, \
        .gpio = (_gpio), \
        .pin = (uint32_t)(_pin), \
        .muxRegister = (uint32_t)&(IOMUXC->SW_MUX_CTL_PAD[kIOMUXC_SW_MUX_CTL_PAD_##_name]), \
        .configRegister = (uint32_t)&(IOMUXC->SW_PAD_CTL_PAD[kIOMUXC_SW_PAD_CTL_PAD_##_name]), \
        .mode = PIN_MODE_IN, \
        .af_mode = PIN_AF_MODE_ALT5, \
        .af_list_len = (size_t)(sizeof((_af_list)) / sizeof(pin_af_obj_t)), \
        .af_list = (_af_list), \
    } \

// Include board specific pins
#include "pins.h"

#endif // MICROPY_INCLUDED_MIMXRT_PIN_H
