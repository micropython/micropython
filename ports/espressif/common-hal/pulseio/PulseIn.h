// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"
#include "driver/rmt_types.h"

typedef struct {
    mp_obj_base_t base;

    const mcu_pin_obj_t *pin;
    rmt_channel_handle_t channel;

    uint16_t *buffer;
    size_t maxlen;

    rmt_symbol_word_t *raw_symbols;
    size_t raw_symbols_size;

    volatile uint16_t start;
    volatile uint16_t len;

    bool idle_state;
    bool paused;
    bool find_first;
} pulseio_pulsein_obj_t;
