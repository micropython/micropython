// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/audioio/AudioOut.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/audiocore/RawSample.h"

extern const mp_obj_type_t audioio_audioout_type;

// left_channel will always be non-NULL but right_channel may be for mono output.
void common_hal_audioio_audioout_construct(audioio_audioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel, uint16_t default_value);

void common_hal_audioio_audioout_deinit(audioio_audioout_obj_t *self);
bool common_hal_audioio_audioout_deinited(audioio_audioout_obj_t *self);
void common_hal_audioio_audioout_play(audioio_audioout_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audioio_audioout_stop(audioio_audioout_obj_t *self);
bool common_hal_audioio_audioout_get_playing(audioio_audioout_obj_t *self);
void common_hal_audioio_audioout_pause(audioio_audioout_obj_t *self);
void common_hal_audioio_audioout_resume(audioio_audioout_obj_t *self);
bool common_hal_audioio_audioout_get_paused(audioio_audioout_obj_t *self);
