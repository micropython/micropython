/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "peripherals/events.h"
//
// #include "clocks.h"
//
#include "py/runtime.h"

uint8_t find_async_event_channel(void) {
    int8_t channel;
    for (channel = EVSYS_CHANNELS - 1; channel > 0; channel--) {
        if (event_channel_free(channel)) {
            break;
        }
    }
    if (channel < 0) {
        mp_raise_RuntimeError("All event channels in use");
    }
    return channel;
}

#ifdef SAMD21
#define EVSYS_SYNCH_NUM EVSYS_CHANNELS
#endif
uint8_t find_sync_event_channel(void) {
    uint8_t channel;
    for (channel = 0; channel < EVSYS_SYNCH_NUM; channel++) {
        if (event_channel_free(channel)) {
            break;
        }
    }
    if (channel >= EVSYS_SYNCH_NUM) {
        mp_raise_RuntimeError("All sync event channels in use");
    }
    return channel;
}
