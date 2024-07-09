// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/rotaryio/IncrementalEncoder.h"

void shared_module_softencoder_state_init(rotaryio_incrementalencoder_obj_t *self, uint8_t quiescent_state);
void shared_module_softencoder_state_update(rotaryio_incrementalencoder_obj_t *self, uint8_t new_state);
mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t *self);
void common_hal_rotaryio_incrementalencoder_set_position(rotaryio_incrementalencoder_obj_t *self, mp_int_t position);
