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

#include "timers.h"

#include "common-hal/pulseio/PulseOut.h"

#ifdef SAMD21
#include "hpl/gclk/hpl_gclk_base.h"
#endif

#ifdef SAMD51
#include "hri/hri_gclk_d51.h"
#endif

const uint16_t prescaler[8] = {1, 2, 4, 8, 16, 64, 256, 1024};

// This bitmask keeps track of which channels of a TCC are currently claimed.
#ifdef SAMD21
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
#endif
#ifdef SAMD51
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
#endif
Tc* const tc_insts[TC_INST_NUM] = TC_INSTS;
Tcc* const tcc_insts[TCC_INST_NUM] = TCC_INSTS;

IRQn_Type const tc_irq[TC_INST_NUM] = {
#ifdef TC0
    TC0_IRQn,
#endif
#ifdef TC1
    TC1_IRQn,
#endif
#ifdef TC2
    TC2_IRQn,
#endif
#ifdef TC3
    TC3_IRQn,
#endif
#ifdef TC4
    TC4_IRQn,
#endif
#ifdef TC5
    TC5_IRQn,
#endif
#ifdef TC6
    TC6_IRQn,
#endif
#ifdef TC7
    TC7_IRQn,
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
    #ifdef SAMD51
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
    #endif

    #ifdef SAMD21
    // Determine the clock slot on the APBC bus. TCC0 is the first and 8 slots in.
    uint8_t clock_slot = 8 + index;
    // We index TCs starting at zero but in memory they begin at three so we have to add three.
    if (is_tc) {
        clock_slot += 3;
    }
    PM->APBCMASK.reg |= 1 << clock_slot;
    _gclk_enable_channel(gclk_id, gclk_index);
    #endif
}

void tc_set_enable(Tc* tc, bool enable) {
    tc->COUNT16.CTRLA.bit.ENABLE = enable;
    #ifdef SAMD21
    while (tc->COUNT16.STATUS.bit.SYNCBUSY != 0) {
        /* Wait for sync */
    }
    #endif
    #ifdef SAMD51
    while (tc->COUNT16.SYNCBUSY.bit.ENABLE != 0) {
        /* Wait for sync */
    }
    #endif
}

void tc_enable_interrupts(uint8_t tc_index) {
    NVIC_DisableIRQ(tc_irq[tc_index]);
    NVIC_ClearPendingIRQ(tc_irq[tc_index]);
    NVIC_EnableIRQ(tc_irq[tc_index]);
}

void tc_disable_interrupts(uint8_t tc_index) {
    NVIC_DisableIRQ(tc_irq[tc_index]);
    NVIC_ClearPendingIRQ(tc_irq[tc_index]);
}

void tcc_set_enable(Tcc* tcc, bool enable) {
    tcc->CTRLA.bit.ENABLE = enable;
    while (tcc->SYNCBUSY.bit.ENABLE != 0) {
        /* Wait for sync */
    }
}

void tc_wait_for_sync(Tc* tc) {
    #ifdef SAMD21
    while (tc->COUNT16.STATUS.bit.SYNCBUSY != 0) {}
    #endif
    #ifdef SAMD51
    while (tc->COUNT16.SYNCBUSY.reg != 0) {}
    #endif
}

void tc_reset(Tc* tc) {
    tc->COUNT16.CTRLA.bit.SWRST = 1;
    while (tc->COUNT16.CTRLA.bit.SWRST == 1) {
    }
}

void shared_timer_handler(bool is_tc, uint8_t index) {
    // Add calls to interrupt handlers for specific functionality here.
    if (is_tc) {
        pulseout_interrupt_handler(index);
    }
}

#ifdef SAMD51
#define TC_OFFSET 0
#endif
#ifdef SAMD21
#define TC_OFFSET 3
#endif

void TCC0_Handler(void) {
    shared_timer_handler(false, 0);
}
void TCC1_Handler(void) {
    shared_timer_handler(false, 1);
}
void TCC2_Handler(void) {
    shared_timer_handler(false, 2);
}
// TC0 - TC2 only exist on the SAMD51
#ifdef TC0
void TC0_Handler(void) {
    shared_timer_handler(true, 0);
}
#endif
#ifdef TC1
void TC1_Handler(void) {
    shared_timer_handler(true, 1);
}
#endif
#ifdef TC2
void TC2_Handler(void) {
    shared_timer_handler(true, 2);
}
#endif
void TC3_Handler(void) {
    shared_timer_handler(true, 3 - TC_OFFSET);
}
void TC4_Handler(void) {
    shared_timer_handler(true, 4 - TC_OFFSET);
}
void TC5_Handler(void) {
    shared_timer_handler(true, 5 - TC_OFFSET);
}
#ifdef TC6
void TC6_Handler(void) {
    shared_timer_handler(true, 6 - TC_OFFSET);
}
#endif
#ifdef TC7
void TC7_Handler(void) {
    shared_timer_handler(true, 7 - TC_OFFSET);
}
#endif
