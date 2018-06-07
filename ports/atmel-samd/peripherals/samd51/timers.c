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

#include "hri/hri_gclk_d51.h"

const uint8_t tcc_cc_num[5] = {6, 4, 3, 2, 2};
const uint8_t tc_gclk_ids[TC_INST_NUM] = {TC0_GCLK_ID,
                                          TC1_GCLK_ID,
                                          TC2_GCLK_ID,
                                          TC3_GCLK_ID,
#ifdef TC4_GCLK_ID
                                          TC4_GCLK_ID,
#endif
#ifdef TC5_GCLK_ID
                                          TC5_GCLK_ID,
#endif
#ifdef TC6_GCLK_ID
                                          TC6_GCLK_ID,
#endif
#ifdef TC7_GCLK_ID
                                          TC7_GCLK_ID,
#endif
                                      };
const uint8_t tcc_gclk_ids[TCC_INST_NUM] = {TCC0_GCLK_ID,
                                            TCC1_GCLK_ID,
                                            TCC2_GCLK_ID,
#ifdef TCC3_GCLK_ID
                                            TCC3_GCLK_ID,
#endif
#ifdef TCC4_GCLK_ID
                                            TCC4_GCLK_ID
#endif
                                    };

void turn_on_clocks(bool is_tc, uint8_t index, uint32_t gclk_index) {
    uint8_t gclk_id;
    if (is_tc) {
        gclk_id = tc_gclk_ids[index];
    } else {
        gclk_id = tcc_gclk_ids[index];
    }
    // Turn on the clocks for the peripherals.
    if (is_tc) {
        switch (index) {
            case 0:
                MCLK->APBAMASK.reg |= MCLK_APBAMASK_TC0;
                break;
            case 1:
                MCLK->APBAMASK.reg |= MCLK_APBAMASK_TC1;
                break;
            case 2:
                MCLK->APBBMASK.reg |= MCLK_APBBMASK_TC2;
                break;
            case 3:
                MCLK->APBBMASK.reg |= MCLK_APBBMASK_TC3;
                break;
            case 4:
                MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC4;
                break;
            case 5:
                MCLK->APBCMASK.reg |= MCLK_APBCMASK_TC5;
                break;
            case 6:
                MCLK->APBDMASK.reg |= MCLK_APBDMASK_TC6;
                break;
            case 7:
                MCLK->APBDMASK.reg |= MCLK_APBDMASK_TC7;
                break;
            default:
                break;
        }
    } else {
        switch (index) {
            case 0:
                MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC0;
                break;
            case 1:
                MCLK->APBBMASK.reg |= MCLK_APBBMASK_TCC1;
                break;
            case 2:
                MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC2;
                break;
            case 3:
                MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC3;
                break;
            case 4:
                MCLK->APBDMASK.reg |= MCLK_APBDMASK_TCC4;
                break;
            default:
                break;
        }
    }

    // FIXME(tannewt): TC4-TC7 can only have 100mhz inputs.

    hri_gclk_write_PCHCTRL_reg(GCLK, gclk_id,
                               gclk_index | (1 << GCLK_PCHCTRL_CHEN_Pos));
}

void tc_set_enable(Tc* tc, bool enable) {
    tc->COUNT16.CTRLA.bit.ENABLE = enable;
    while (tc->COUNT16.SYNCBUSY.bit.ENABLE != 0) {
        /* Wait for sync */
    }
}

void tc_wait_for_sync(Tc* tc) {
    while (tc->COUNT16.SYNCBUSY.reg != 0) {}
}
