// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dave Putz for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/pwmio/PWMOut.h"
#include "src/common/pico_time/include/pico/time.h"

#include "py/obj.h"

#define NO_PIN 0xff

typedef struct {
    mp_obj_base_t base;
    uint8_t pin;
    uint8_t slice;
    pwmio_pwmout_obj_t carrier;
    uint16_t *pulse_buffer;
    uint16_t pulse_length;
    uint32_t min_pulse;
    volatile uint16_t pulse_index;
} pulseio_pulseout_obj_t;

int64_t pulseout_interrupt_handler(alarm_id_t id, void *user_data);
