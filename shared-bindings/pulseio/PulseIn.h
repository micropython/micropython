// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pulseio/PulseIn.h"

extern const mp_obj_type_t pulseio_pulsein_type;

extern void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t maxlen, bool idle_state);
extern void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t *self);
extern bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t *self);
extern void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t *self);
extern void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t *self, uint16_t trigger_duration);
extern void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t *self);
extern uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t *self);
extern uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t *self);
extern bool common_hal_pulseio_pulsein_get_paused(pulseio_pulsein_obj_t *self);
extern uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t *self);
extern uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t *self, int16_t index);
