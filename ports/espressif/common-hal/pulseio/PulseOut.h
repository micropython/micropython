// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "driver/rmt_types.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
} pulseio_pulseout_obj_t;
