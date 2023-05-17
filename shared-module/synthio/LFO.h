/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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

#pragma once

#include "py/obj.h"

#include "shared-module/synthio/__init__.h"
#include "shared-bindings/synthio/__init__.h"

typedef struct synthio_lfo_slot {
    mp_obj_t obj;
    mp_float_t value;
} synthio_lfo_slot_t;

typedef struct synthio_lfo_obj {
    mp_obj_base_t base;

    int32_t sample_rate;

    uint8_t last_tick;
    bool once;

    synthio_lfo_slot_t rate, scale, offset;
    mp_float_t accum, value;

    mp_obj_t waveform_obj;
    mp_buffer_info_t waveform_bufinfo;
} synthio_lfo_obj_t;

// Update the value inside the lfo slot if the value is an LFO, returning the new value
mp_float_t synthio_lfo_obj_tick(synthio_lfo_slot_t *lfo_slot);
// the same, but the output is constrained to be between lo and hi
mp_float_t synthio_lfo_obj_tick_limited(synthio_lfo_slot_t *lfo_slot, mp_float_t lo, mp_float_t hi);
// the same, but the output is constrained to be between lo and hi and converted to an integer with 15 fractional bits
int32_t synthio_lfo_obj_tick_scaled(synthio_lfo_slot_t *lfo_slot, mp_float_t lo, mp_float_t hi);

// Assign an object (which may be a float or a synthio_lfo_obj_t) to an lfo slot
void synthio_lfo_assign_input(mp_obj_t obj, synthio_lfo_slot_t *lfo_slot, qstr arg_name);
