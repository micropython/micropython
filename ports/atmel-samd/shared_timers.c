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

#include <stdint.h>

#include "samd/timers.h"

#include "shared_timers.h"

static uint8_t never_reset_tc_or_tcc[TC_INST_NUM + TCC_INST_NUM];

static void timer_refcount(int index, bool is_tc, int increment) {
    if (is_tc) {
        never_reset_tc_or_tcc[index] += increment;
    } else {
        never_reset_tc_or_tcc[TC_INST_NUM + index] += increment;
    }
}

void timer_never_reset(int index, bool is_tc) {
    timer_refcount(index, is_tc, 1);
}

void timer_reset_ok(int index, bool is_tc) {
    timer_refcount(index, is_tc, -1);
}

bool timer_ok_to_reset(int index, bool is_tc) {
    if (is_tc) {
        return never_reset_tc_or_tcc[index] == 0;
    }
    return never_reset_tc_or_tcc[TC_INST_NUM + index] == 0;
}

void reset_timers(void) {
    // Reset all timers
    Tcc *tccs[TCC_INST_NUM] = TCC_INSTS;
    for (int i = 0; i < TCC_INST_NUM; i++) {
        if (!timer_ok_to_reset(i, false)) {
            continue;
        }
        // Disable the module before resetting it.
        if (tccs[i]->CTRLA.bit.ENABLE == 1) {
            tccs[i]->CTRLA.bit.ENABLE = 0;
            while (tccs[i]->SYNCBUSY.bit.ENABLE == 1) {
            }
        }
        tccs[i]->CTRLA.bit.SWRST = 1;
        while (tccs[i]->CTRLA.bit.SWRST == 1) {
        }
    }
    Tc *tcs[TC_INST_NUM] = TC_INSTS;
    for (int i = 0; i < TC_INST_NUM; i++) {
        if (!timer_ok_to_reset(i, true)) {
            continue;
        }
        tcs[i]->COUNT16.CTRLA.bit.SWRST = 1;
        while (tcs[i]->COUNT16.CTRLA.bit.SWRST == 1) {
        }
    }
}
