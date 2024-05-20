// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/audiobusio/I2SOut.h"
#include "common-hal/microcontroller/Pin.h"

extern const mp_obj_type_t audiobusio_i2sout_type;

// Some boards don't have the I2SOut pins available.
#if CIRCUITPY_AUDIOBUSIO_I2SOUT

void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select, const mcu_pin_obj_t *data,
    const mcu_pin_obj_t *main_clock, bool left_justified);

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self);
bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self);
void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self);
bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self);
void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self);
void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self);
bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self);

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT
