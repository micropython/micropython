// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/pwmio/PWMOut.h"

extern const mp_obj_type_t pulseio_pulseout_type;

extern void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle);

extern void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self);
extern bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self);
extern void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self,
    uint16_t *pulses, uint16_t len);
