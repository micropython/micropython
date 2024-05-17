// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint8_t slice;        // 0-7
    uint8_t ab_channel;   // 0-1: A or B slice channel
    bool variable_frequency;
    uint16_t duty_cycle;
    uint32_t actual_frequency;
    uint16_t top;
} pwmio_pwmout_obj_t;

// Private API for AudioPWMOut.
void pwmio_pwmout_set_top(pwmio_pwmout_obj_t *self, uint16_t top);
// Private APIs for RGBMatrix
enum pwmout_result_t pwmout_allocate(uint8_t slice, uint8_t ab_channel, bool variable_frequency, uint32_t frequency);
void pwmout_free(uint8_t slice, uint8_t ab_channel);

// Private API for countio to claim both ab_channels on a slice
bool pwmio_claim_slice_ab_channels(uint8_t slice);
void pwmio_release_slice_ab_channels(uint8_t slice);
