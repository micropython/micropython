/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
