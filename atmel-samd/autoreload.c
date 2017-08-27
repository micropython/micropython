/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "autoreload.h"

#include "asf/sam0/drivers/tc/tc_interrupt.h"
#include "lib/utils/interrupt_char.h"
#include "py/mphal.h"

volatile uint32_t autoreload_delay_ms = 0;
bool autoreload_enabled = false;
volatile bool reload_next_character = false;

inline void autoreload_tick() {
    if (autoreload_delay_ms == 0) {
        return;
    }
    if (autoreload_delay_ms == 1 && autoreload_enabled && !reload_next_character) {
        mp_keyboard_interrupt();
        reload_next_character = true;
    }
    autoreload_delay_ms--;
}

void autoreload_enable() {
    autoreload_enabled = true;
    reload_next_character = false;
}

void autoreload_disable() {
    autoreload_enabled = false;
}

inline bool autoreload_is_enabled() {
    return autoreload_enabled;
}

void autoreload_start() {
    autoreload_delay_ms = CIRCUITPY_AUTORELOAD_DELAY_MS;
}

void autoreload_stop() {
    autoreload_delay_ms = 0;
    reload_next_character = false;
}
