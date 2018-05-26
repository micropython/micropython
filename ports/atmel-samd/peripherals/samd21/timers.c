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

#include "peripherals/timers.h"

//#include "common-hal/pulseio/PulseOut.h"

#include "hpl/gclk/hpl_gclk_base.h"

const uint8_t tcc_cc_num[3] = {4, 2, 2};
const uint8_t tc_gclk_ids[TC_INST_NUM] = {TC3_GCLK_ID,
               TC4_GCLK_ID,
               TC5_GCLK_ID,
#ifdef TC6_GCLK_ID
               TC6_GCLK_ID,
#endif
#ifdef TC7_GCLK_ID
               TC7_GCLK_ID,
#endif
            };
const uint8_t tcc_gclk_ids[3] = {TCC0_GCLK_ID, TCC1_GCLK_ID, TCC2_GCLK_ID};

void turn_on_clocks(bool is_tc, uint8_t index, uint32_t gclk_index) {
    uint8_t gclk_id;
    if (is_tc) {
        gclk_id = tc_gclk_ids[index];
    } else {
        gclk_id = tcc_gclk_ids[index];
    }
    // Determine the clock slot on the APBC bus. TCC0 is the first and 8 slots in.
    uint8_t clock_slot = 8 + index;
    // We index TCs starting at zero but in memory they begin at three so we have to add three.
    if (is_tc) {
        clock_slot += 3;
    }
    PM->APBCMASK.reg |= 1 << clock_slot;
    _gclk_enable_channel(gclk_id, gclk_index);
}

void tc_set_enable(Tc* tc, bool enable) {
    tc->COUNT16.CTRLA.bit.ENABLE = enable;
    while (tc->COUNT16.STATUS.bit.SYNCBUSY != 0) {
        /* Wait for sync */
    }
}

void tc_wait_for_sync(Tc* tc) {
    while (tc->COUNT16.STATUS.bit.SYNCBUSY != 0) {}
}
