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

#pragma once

#include "sdk/drivers/sai/fsl_sai.h"
#include "py/obj.h"

#include "supervisor/background_callback.h"


typedef struct {
    I2S_Type *peripheral;
    sai_handle_t handle;
    mp_obj_t sample;
    uint32_t *buffers[SAI_XFER_QUEUE_SIZE];
    uint8_t *sample_data, *sample_end;
    background_callback_t callback;
    bool playing, paused, loop, stopping;
    bool samples_signed;
    uint8_t channel_count, bytes_per_sample;
    uint8_t buffer_idx;
    uint32_t sample_rate;
} i2s_t;


void i2s_reset(void);
void port_i2s_initialize(i2s_t *self, int instance, sai_transceiver_t *config);
void port_i2s_deinit(i2s_t *self);
bool port_i2s_deinited(i2s_t *self);
void port_i2s_play(i2s_t *self, mp_obj_t sample, bool loop);
void port_i2s_stop(i2s_t *self);
bool port_i2s_get_playing(i2s_t *self);
bool port_i2s_get_paused(i2s_t *self);
void port_i2s_pause(i2s_t *self);
void port_i2s_resume(i2s_t *self);
