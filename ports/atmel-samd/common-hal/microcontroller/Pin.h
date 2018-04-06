/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_MICROCONTROLLER_PIN_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_MICROCONTROLLER_PIN_H

#include "py/obj.h"

#include "mpconfigport.h"

#include "include/component/sercom.h"

typedef struct {
    uint8_t index:6;            // 0, 1, etc. corresponding to SERCOM<n>.
    uint8_t pad:2;              // which of the four SERCOM pads to use
} pin_sercom_t;

typedef struct {
    uint8_t index;
    bool is_tc:1;
    uint8_t wave_output:7;
} pin_timer_t;

#ifdef SAMD21
    #define NUM_TIMERS_PER_PIN 2
    #define NUM_ADC_PER_PIN 1
#endif
#ifdef SAMD51
    #define NUM_TIMERS_PER_PIN 3
    #define NUM_ADC_PER_PIN 2
#endif
#define NUM_SERCOMS_PER_PIN 2

typedef struct {
    mp_obj_base_t base;
    qstr name;
    uint8_t pin;
    bool has_extint:1;
    uint8_t extint_channel:7;
    bool has_touch:1;
    uint8_t touch_y_line:7; // 0 - 15. Assumed to be Y channel.
    uint8_t adc_input[NUM_ADC_PER_PIN];
    pin_timer_t timer[NUM_TIMERS_PER_PIN];
    pin_sercom_t sercom[NUM_SERCOMS_PER_PIN];
} mcu_pin_obj_t;

#ifdef MICROPY_HW_NEOPIXEL
extern bool neopixel_in_use;
#endif
#ifdef MICROPY_HW_APA102_MOSI
extern bool apa102_sck_in_use;
extern bool apa102_mosi_in_use;
#endif

void reset_all_pins(void);
// reset_pin takes the pin number instead of the pointer so that objects don't
// need to store a full pointer.
void reset_pin(uint8_t pin);
void claim_pin(const mcu_pin_obj_t* pin);

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_MICROCONTROLLER_PIN_H
