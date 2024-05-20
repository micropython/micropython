// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Michael Schroeder
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/frequencyio/FrequencyIn.h"

extern const mp_obj_type_t frequencyio_frequencyin_type;

extern void common_hal_frequencyio_frequencyin_construct(frequencyio_frequencyin_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t capture_period);
extern void common_hal_frequencyio_frequencyin_deinit(frequencyio_frequencyin_obj_t *self);
extern bool common_hal_frequencyio_frequencyin_deinited(frequencyio_frequencyin_obj_t *self);
extern void common_hal_frequencyio_frequencyin_pause(frequencyio_frequencyin_obj_t *self);
extern void common_hal_frequencyio_frequencyin_resume(frequencyio_frequencyin_obj_t *self);
extern void common_hal_frequencyio_frequencyin_clear(frequencyio_frequencyin_obj_t *self);
extern uint32_t common_hal_frequencyio_frequencyin_get_item(frequencyio_frequencyin_obj_t *self);
extern uint16_t common_hal_frequencyio_frequencyin_get_capture_period(frequencyio_frequencyin_obj_t *self);
extern void common_hal_frequencyio_frequencyin_set_capture_period(frequencyio_frequencyin_obj_t *self, uint16_t capture_period);
