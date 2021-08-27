/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include <stdint.h>
#include <string.h>

#include "mpconfigport.h"

#include "bindings/espidf/__init__.h"

// Some boards don't implement I2SOut, so suppress any routines from here.
#if CIRCUITPY_AUDIOBUSIO_I2SOUT

#include "extmod/vfs_fat.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audiobusio/I2SOut.h"
#include "shared-bindings/audiobusio/I2SOut.h"
#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"

#include "driver/i2s.h"

// Caller validates that pins are free.
void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, bool left_justified) {

    port_i2s_allocate_init(&self->peripheral, left_justified);

    i2s_pin_config_t i2s_pin_config = {
        .bck_io_num = bit_clock->number,
        .ws_io_num = word_select->number,
        .data_out_num = data->number,
        .data_in_num = I2S_PIN_NO_CHANGE,
    };
    CHECK_ESP_RESULT(i2s_set_pin(self->peripheral.instance, &i2s_pin_config));
    self->bit_clock = bit_clock;
    self->word_select = word_select;
    self->data = data;
    claim_pin(bit_clock);
    claim_pin(word_select);
    claim_pin(data);
}

bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self) {
    return self->peripheral.instance == -1;
}

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self) {
    if (common_hal_audiobusio_i2sout_deinited(self)) {
        return;
    }

    if (self->bit_clock) {
        reset_pin_number(self->bit_clock->number);
    }
    self->bit_clock = NULL;

    if (self->word_select) {
        reset_pin_number(self->word_select->number);
    }
    self->word_select = NULL;

    if (self->data) {
        reset_pin_number(self->data->number);
    }
    self->data = NULL;

    if (self->peripheral.instance >= 0) {
        port_i2s_reset_instance(self->peripheral.instance);
    }
    self->peripheral.instance = -1;
}

void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audiobusio_i2sout_get_playing(self)) {
        common_hal_audiobusio_i2sout_stop(self);
    }
    port_i2s_play(&self->peripheral, sample, loop);
}

void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self) {
    port_i2s_pause(&self->peripheral);
}

void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self) {
    port_i2s_resume(&self->peripheral);
}

bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self) {
    return port_i2s_paused(&self->peripheral);
}

void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self) {
    port_i2s_stop(&self->peripheral);
}

bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self) {
    return port_i2s_playing(&self->peripheral);
}

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT
