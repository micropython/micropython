// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

#include "timer_handler.h"

#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/_pew/PewPew.h"
#include "common-hal/frequencyio/FrequencyIn.h"

extern void _PM_IRQ_HANDLER(void);

static uint8_t tc_handler[TC_INST_NUM];

void set_timer_handler(bool is_tc, uint8_t index, uint8_t timer_handler) {
    if (is_tc) {
        tc_handler[index] = timer_handler;
    }
}

void shared_timer_handler(bool is_tc, uint8_t index) {
    // Add calls to interrupt handlers for specific functionality here.
    // Make sure to add the handler #define to timer_handler.h
    if (is_tc) {
        uint8_t handler = tc_handler[index];
        switch (handler) {
            case TC_HANDLER_PULSEIN:
                #if CIRCUITPY_PULSEIO
                pulsein_timer_interrupt_handler(index);
                #endif
                break;
            case TC_HANDLER_PULSEOUT:
                #if CIRCUITPY_PULSEIO
                pulseout_interrupt_handler(index);
                #endif
                break;
            case TC_HANDLER_PEW:
                #if CIRCUITPY_PEW
                pewpew_interrupt_handler(index);
                #endif
                break;
            case TC_HANDLER_FREQUENCYIN:
                #if CIRCUITPY_FREQUENCYIO
                frequencyin_interrupt_handler(index);
                #endif
                break;
            case TC_HANDLER_RGBMATRIX:
                #if CIRCUITPY_RGBMATRIX
                _PM_IRQ_HANDLER();
                #endif
                break;
            default:
                break;
        }
    }
}
