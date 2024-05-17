// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
