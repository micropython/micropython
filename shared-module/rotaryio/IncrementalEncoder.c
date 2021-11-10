/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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

#if CIRCUITPY_ROTARYIO && CIRCUITPY_ROTARYIO_SOFTENCODER
#include "shared-bindings/rotaryio/IncrementalEncoder.h"
#include "shared-module/rotaryio/IncrementalEncoder.h"
#include "common-hal/rotaryio/IncrementalEncoder.h"

void shared_module_softencoder_state_init(rotaryio_incrementalencoder_obj_t *self, uint8_t quiescent_state) {
    self->state = quiescent_state;
    self->sub_count = 0;
    common_hal_rotaryio_incrementalencoder_set_position(self, 0);
}

void shared_module_softencoder_state_update(rotaryio_incrementalencoder_obj_t *self, uint8_t new_state) {
    static const int8_t transitions[16] = {
        0,    // 00 -> 00 no movement
        -1,   // 00 -> 01 3/4 ccw (11 detent) or 1/4 ccw (00 at detent)
        +1,   // 00 -> 10 3/4 cw or 1/4 cw
        0,     // 00 -> 11 non-Gray-code transition
        +1,   // 01 -> 00 2/4 or 4/4 cw
        0,    // 01 -> 01 no movement
        0,    // 01 -> 10 non-Gray-code transition
        -1,   // 01 -> 11 4/4 or 2/4 ccw
        -1,   // 10 -> 00 2/4 or 4/4 ccw
        0,    // 10 -> 01 non-Gray-code transition
        0,    // 10 -> 10 no movement
        +1,   // 10 -> 11 4/4 or 2/4 cw
        0,    // 11 -> 00 non-Gray-code transition
        +1,   // 11 -> 01 1/4 or 3/4 cw
        -1,   // 11 -> 10 1/4 or 3/4 ccw
        0,    // 11 -> 11 no movement
    };

    new_state &= 0x3;
    int idx = (self->state << 2) | new_state;
    self->state = new_state;

    int8_t sub_incr = transitions[idx];

    self->sub_count += sub_incr;

    if (self->sub_count >= self->divisor) {
        self->position += 1;
        self->sub_count = 0;
    } else if (self->sub_count <= -self->divisor) {
        self->position -= 1;
        self->sub_count = 0;
    }
}

mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t *self) {
    return self->position;
}

void common_hal_rotaryio_incrementalencoder_set_position(rotaryio_incrementalencoder_obj_t *self, mp_int_t position) {
    self->position = position;
}

mp_int_t common_hal_rotaryio_incrementalencoder_get_divisor(rotaryio_incrementalencoder_obj_t *self) {
    return self->divisor;
}

void common_hal_rotaryio_incrementalencoder_set_divisor(rotaryio_incrementalencoder_obj_t *self, mp_int_t divisor) {
    self->divisor = divisor;
}
#endif
