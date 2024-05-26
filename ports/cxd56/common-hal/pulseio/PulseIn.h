// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2019 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint16_t *buffer;
    uint16_t maxlen;
    uint16_t start;
    uint16_t len;
    uint64_t last_us;
    bool idle_state;
    bool first_edge;
    bool paused;
} pulseio_pulsein_obj_t;
