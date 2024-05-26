// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dave Putz for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "src/rp2_common/hardware_pio/include/hardware/pio.h"
#include "common-hal/rp2pio/StateMachine.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t pin;
    bool idle_state;
    bool paused;
    uint16_t maxlen;
    uint16_t *buffer;
    volatile bool last_level;
    volatile uint32_t level_count;
    volatile uint16_t len;
    volatile uint16_t start;
    rp2pio_statemachine_obj_t state_machine;
} pulseio_pulsein_obj_t;

void common_hal_pulseio_pulsein_interrupt(void *);
