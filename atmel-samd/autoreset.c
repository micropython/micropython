/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "autoreset.h"

#include "asf/sam0/drivers/tc/tc_interrupt.h"
#include "lib/utils/interrupt_char.h"
#include "py/mphal.h"

volatile uint32_t autoreset_delay_ms = 0;
bool autoreset_enabled = false;
volatile bool reset_next_character = false;

inline void autoreset_tick() {
    if (autoreset_delay_ms == 0) {
        return;
    }
    if (autoreset_delay_ms == 1 && autoreset_enabled && !reset_next_character) {
        mp_keyboard_interrupt();
        reset_next_character = true;
    }
    autoreset_delay_ms--;
}

void autoreset_enable() {
    autoreset_enabled = true;
    reset_next_character = false;
}

void autoreset_disable() {
    autoreset_enabled = false;
}

inline bool autoreset_is_enabled() {
    return autoreset_enabled;
}

void autoreset_start() {
    autoreset_delay_ms = AUTORESET_DELAY_MS;
}

void autoreset_stop() {
    autoreset_delay_ms = 0;
}
