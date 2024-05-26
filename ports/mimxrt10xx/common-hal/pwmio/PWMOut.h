// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "periph.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    const mcu_pwm_obj_t *pwm;
    bool variable_frequency;
    uint8_t flexpwm_index;
    uint8_t prescaler;
    uint16_t duty_cycle;
    uint16_t pulse_count;
} pwmio_pwmout_obj_t;
