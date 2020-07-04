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

#ifndef MICROPY_INCLUDED_MIMXRT_LED_H
#define MICROPY_INCLUDED_MIMXRT_LED_H

#include "pin.h"

#if defined(MICROPY_HW_LED1_PIN)
#define NUM_LEDS (1)
#else
#define NUM_LEDS (0)
#endif

typedef enum {
    MACHINE_BOARD_LED = 1,
} machine_led_t;

typedef struct _machine_led_obj_t {
    mp_obj_base_t base;
    mp_uint_t led_id;
    const pin_obj_t *led_pin;
} machine_led_obj_t;

void led_init(void);
void led_state(machine_led_t led, int state);
void led_toggle(machine_led_t led);
void led_debug(int value, int delay);

extern const mp_obj_type_t machine_led_type;
extern const machine_led_obj_t machine_led_obj[NUM_LEDS];

#endif // MICROPY_INCLUDED_MIMXRT_LED_H
