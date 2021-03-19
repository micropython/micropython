/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/ps2io/Ps2.h"
#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "common-hal/countio/Counter.h"
#include "shared-bindings/microcontroller/__init__.h"
// #include "samd/external_interrupts.h"
#include "eic_handler.h"

// Which handler should be called for a particular channel?
static uint8_t eic_channel_handler[EIC_EXTINT_NUM];

void set_eic_handler(uint8_t channel, uint8_t eic_handler) {
    eic_channel_handler[channel] = eic_handler;
}

void shared_eic_handler(uint8_t channel) {
    uint8_t handler = eic_channel_handler[channel];
    switch (handler) {
        #if CIRCUITPY_PULSEIO
        case EIC_HANDLER_PULSEIN:
            pulsein_interrupt_handler(channel);
            break;
        #endif

        #if CIRCUITPY_PS2IO
        case EIC_HANDLER_PS2:
            ps2_interrupt_handler(channel);
            break;
        #endif

        #if CIRCUITPY_ROTARYIO
        case EIC_HANDLER_INCREMENTAL_ENCODER:
            incrementalencoder_interrupt_handler(channel);
            break;
        #endif

        #if CIRCUITPY_COUNTIO
        case EIC_HANDLER_COUNTER:
            counter_interrupt_handler(channel);
            break;
        #endif

        default:
            break;
    }
}
