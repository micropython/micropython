/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#include "common-hal/audiobusio/PDMIn.h"

#include "py/runtime.h"

void common_hal_audiobusio_pdmin_construct(audiobusio_pdmin_obj_t* self,
                                           const mcu_pin_obj_t* clock_pin,
                                           const mcu_pin_obj_t* data_pin,
                                           uint32_t sample_rate,
                                           uint8_t bit_depth,
                                           bool mono,
                                           uint8_t oversample) {
    mp_raise_RuntimeError(translate("not implemented"));
}

bool common_hal_audiobusio_pdmin_deinited(audiobusio_pdmin_obj_t* self) {
    mp_raise_RuntimeError(translate("not implemented"));
}

void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t* self) {
    mp_raise_RuntimeError(translate("not implemented"));
}

uint8_t common_hal_audiobusio_pdmin_get_bit_depth(audiobusio_pdmin_obj_t* self) {
    return self->bit_depth;
}

uint32_t common_hal_audiobusio_pdmin_get_sample_rate(audiobusio_pdmin_obj_t* self) {
    return self->sample_rate;
}

uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t* self,
        uint16_t* output_buffer, uint32_t output_buffer_length) {
    mp_raise_RuntimeError(translate("not implemented"));
}

void common_hal_audiobusio_pdmin_record_to_file(audiobusio_pdmin_obj_t* self, uint8_t* buffer, uint32_t length) {
    mp_raise_RuntimeError(translate("not implemented"));
}
