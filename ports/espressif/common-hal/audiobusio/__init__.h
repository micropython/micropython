// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "supervisor/background_callback.h"

#include "driver/i2s_std.h"

typedef struct {
    mp_obj_t *sample;
    bool left_justified;
    bool loop;
    bool paused; // True when the I2S channel is configured but disabled.
    bool playing; // True when the I2S channel is configured.
    bool stopping;
    bool samples_signed;
    int8_t bytes_per_sample;
    int8_t channel_count;
    uint16_t buffer_length;
    uint8_t *sample_data, *sample_end;
    void *next_buffer;
    size_t next_buffer_size;
    i2s_chan_handle_t handle;
    background_callback_t callback;
    bool underrun;
} i2s_t;


void port_i2s_allocate_init(i2s_t *self, bool left_justified);
void port_i2s_deinit(i2s_t *self);

void port_i2s_play(i2s_t *self, mp_obj_t sample, bool loop);
void port_i2s_stop(i2s_t *self);
bool port_i2s_playing(i2s_t *self);
bool port_i2s_paused(i2s_t *self);
void port_i2s_pause(i2s_t *self);
void port_i2s_resume(i2s_t *self);
